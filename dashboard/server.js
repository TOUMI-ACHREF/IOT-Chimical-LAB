const express = require('express');
const mongoose = require('mongoose');
const bodyParser = require('body-parser');
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const session = require('express-session');

const http = require('http');
const { Server } = require('socket.io');

const app = express();
const port = 3000;

app.set('view engine', 'ejs');

require('dotenv').config();
mongoose.connect(process.env.MONGODB_URI, { useNewUrlParser: true, useUnifiedTopology: true });



const sensorSchema = new mongoose.Schema({
  bme: {
    temperature: Number,
    humidity: Number,
    pressure: Number,
  },
  flame: {
    status: Number,
    analog: Number,
  },
  mq5: {
    value: Number,
  },
  ultrasonic: {
    distance: Number,
  },
  timestamp: { type: Date, default: Date.now },
});

const SensorData = mongoose.model('SensorData', sensorSchema);


// Schema for User Accounts
const accountSchema = new mongoose.Schema({
  fullname: {
    type: String,
    required: true, 
  },
  email: {
    type: String,
    required: true,
    unique: true,
    trim: true,
  },
  password: {
    type: String,
    required: true,
  },
  
});
const Account = mongoose.model('Account', accountSchema);

const alertSchema = new mongoose.Schema({
  type: String,
  message: String,
  details: Object,
  timestamp: { type: Date, default: Date.now },
});

const Alert = mongoose.model('Alert', alertSchema);

// Configuration de session
app.use(
  session({
    secret: "a27f4b9e2a78c9d2e4f5e813f1a2c5678a9b3c4d5e6f7g8h9i0jklmnopqrstuvw", 
    resave: false,
    saveUninitialized: true,
    cookie: { maxAge: 3600000 },
  })
);

// Middleware
app.use(bodyParser.json());
let route = require("./routes/route");

app.use("/", route);

// HTTP server
const server = http.createServer(app);

// Socket.IO
const io = new Server(server);

io.on('connection', (socket) => {
  console.log('Un utilisateur est connecté via WebSocket');
});

// Endpoint to receve data
app.post('/api/data', async (req, res) => {
  try {
    const { bme, flame, mq5, ultrasonic } = req.body;

    const alerts = [];
    if (flame.status === 1) {
      const alert = {
        type: "Feu détecté",
        message: "Une flamme a été détectée ! Veuillez vérifier immédiatement.",
        details: flame,
      };
      alerts.push(alert);
      await Alert.create(alert); 
    }

    if (mq5.value > 400) {
      const alert = {
        type: "Gaz dangereux",
        message: `Le niveau de gaz détecté est critique (${mq5.value}).`,
        details: mq5,
      };
      alerts.push(alert);
      await Alert.create(alert);
    }

    if (ultrasonic.distance < 10) {
      const alert = {
        type: "Distance critique",
        message: `Un objet est détecté à une distance dangereuse (${ultrasonic.distance} cm).`,
        details: ultrasonic,
      };
      alerts.push(alert);
      await Alert.create(alert);
    }


    const sensorData = new SensorData({
      bme: {
        temperature: bme.temperature,
        humidity: bme.humidity,
        pressure: bme.pressure,
      },
      flame: {
        status: flame.status,
        analog: flame.analog,
      },
      mq5: {
        value: mq5.value,
      },
      ultrasonic: {
        distance: ultrasonic.distance,
      },
    });

    await sensorData.save();
    res.status(201).send({ message: 'Données enregistrées avec succès' });
  } catch (error) {
    console.error(error);
    res.status(500).send({ error: 'Erreur au niveau de insertion !!!' });
  }
});


// Endpoint for alerts
app.get('/api/alerts', async (req, res) => {
  try {
    const alerts = await Alert.find().sort({ timestamp: -1 }).limit(50); 
    res.status(200).send(alerts);
  } catch (error) {
    console.error("Erreur lors de la récupération des alertes :", error);
    res.status(500).send({ error: 'Erreur lors de la récupération des alertes' });
  }
});

// Endpoint to fetch data
app.get('/sensorsData', async (req, res) => {
  try {
    const data = await SensorData.find().sort({ timestamp: -1 }).limit(50); 
    res.status(200).send(data);
    console.log('Data fetching SUCCESSFULL !!');
  } catch (error) {
    console.error(error);
    res.status(500).send({ error: 'Error fetching data' });
  }
});

// Login route
app.post('/api/login', async (req, res) => {
  const { email, password } = req.body;

  if (!email || !password) {
    return res.status(400).json({ message: 'Please provide email and password.' });
  }

  try {
    // Find the user by email
    const account = await Account.findOne({ email });
    if (!account) {
      return res.status(400).json({ message: 'Invalid credentials' });
    }

    const isMatch = await bcrypt.compare(password, account.password);
    if (!isMatch) {
      return res.status(400).json({ message: 'Invalid credentials' });
    }

    // Generate JWT token
    const token = jwt.sign({ userId: account._id, email: account.email }, 'your_jwt_secret', { expiresIn: '1h' });

     req.session.user = {
      id: account._id,
      name: account.fullname,
      email: account.email,
    };

    // Send the token back
    res.status(200).json({ message: 'Login successful', token });
  } catch (error) {
    console.error(error);
    res.status(500).json({ error: 'Server error during login' });
  }
});

// Register route
app.post('/api/register', async (req, res) => {
  const { email, password, fullname } = req.body;
  console.log("register function...");
  if (!email || !password || !fullname) {
    return res.status(400).json({ message: 'Please provide email, password, and fullname.' });
  }

  try {
    const existingAccount = await Account.findOne({ email });
    if (existingAccount) {
      return res.status(400).json({ message: 'Email already in use' });
    }

    const hashedPassword = await bcrypt.hash(password, 10);
    
    const newAccount = new Account({
      email,
      password: hashedPassword,
      fullname,
    });

    await newAccount.save();
     
     req.session.user = {
      id: newAccount._id,
      name: newAccount.fullname,
      email: newAccount.email,
    };

    res.status(201).json({ message: 'Account created successfully' });
  } catch (error) {
    res.status(500).json({ message: 'Server error', error });
  }
});

app.get('/account', (req, res) => {
  if (!req.session.user) {
    return res.redirect('/login');
  }
  res.render('account', { user: req.session.user });
});

app.get('/logout', (req, res) => {
  req.session.destroy((err) => {
    if (err) {
      console.error("Erreur lors de la déconnexion :", err);
    }
  });
  res.render('login');
});

app.listen(port, () => {
  console.log(`Serveur démarré sur http://localhost:${port}`);
});

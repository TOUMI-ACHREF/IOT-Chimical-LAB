# 🌟 Chimical LAB: IoT-Based Laboratory Monitoring System

![Project Status](https://img.shields.io/badge/Status-Completed-brightgreen)  
![IoT](https://img.shields.io/badge/Category-IoT-blue)  
![License](https://img.shields.io/badge/License-MIT-yellow)

## **🔓 Overview**  
**Chimical LAB** is an IoT project designed to enhance laboratory safety by monitoring critical environmental parameters. This system uses multiple sensors to detect:  
- 🔥 **Flame presence**  
- 💨 **Gas levels**  
- 🌡️ **Temperature**  
- 🖹️ **Motion** (via an ultrasonic sensor)  

It leverages **MongoDB Atlas** for real-time data storage and provides an intuitive **web dashboard** for visualizing statistics.

---

## **✨ Features**  
- 📡 **Real-time Monitoring**: Continuously track lab conditions.  
- 📾 **Data Storage**: Secure and scalable data management using MongoDB Atlas.  
- 🚨 **Alerts**: Immediate detection of hazardous conditions.  
- 📊 **Statistics and Visualizations**: User-friendly dashboard displaying trends and insights.

---

## **🔧 Components**  
### **1. Hardware**  
- 🖥️ **ESP32 Dev Kit**  
- 🚰 **Gas sensor**  
- 🔥 **Flame sensor**  
- 🌡️ **BME280** (temperature and humidity)  
- 🔏 **Ultrasonic sensor**  

### **2. Software**  
- 🔋 **Arduino code** for sensor data collection  
- 🖥️ **Node.js server** to handle requests and store data  
- ☁️ **MongoDB Atlas** for cloud-based database  
- 🌐 **Web dashboard** for analytics  

---

## **🚀 Setup Instructions**  
1. Clone the repository:  
   ```bash  
   git clone https://github.com/TOUMI-ACHREF/chimical-lab.git  
   ```  
2. Install dependencies for the server:  
   ```bash  
   cd server  
   npm install  
   ```  
3. Add your MongoDB connection string to a `.env` file in the `server` folder:  
   ```plaintext  
   MONGODB_URI=mongodb+srv://username:password@cluster.mongodb.net/dbname  
   ```  
4. Upload the Arduino code to the ESP32 device.  
5. Start the server:  
   ```bash  
   node server.js  
   ```  
6. Access the web dashboard by opening your browser and navigating to:  
   ```
   http://localhost:3000  
   ```  

---

## **📸 Screenshot**  
![Dashboard Screenshot](assets/Screenshot.png)  

`assets/Screenshot.png`

---

## **🗂 Project Structure**  
```plaintext  
chimical-lab/  
│  
├── arduino/                 # Arduino source code  
├── assets/
├── dashboard/               # Dashboard implementation 
│   ├── server.js            # Main server file (NodeJS/Express)
│   ├── package.json         # Dependencies  
│   ├── .env                 # Environment variables  
│   └── routes/              # Views Routes     
│   └── views/               # Web Pages    
├── README.md                # Project documentation  
```  

---

## **🤝 Contributing**  
Contributions are welcome! Feel free to fork the repository, submit a pull request, or open an issue.
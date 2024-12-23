var express = require("express");
let router = express.Router();

router.get("/", (req, res)=>{
    res.render("login.ejs");
});

router.get("/dashboard", (req, res)=>{
    res.render("index.ejs");
});

router.get("/help", (req, res)=>{
    res.render("help.ejs");
});
router.get("/report", (req, res)=>{
    res.render("report.ejs");
});
router.get("/data", (req, res)=>{
    res.render("data.ejs");
});
router.get("/login", (req, res)=>{// /user/:p
    res.render("login.ejs");
});
router.get("/register", (req, res)=>{
    res.render("register.ejs");
});


module.exports = router;
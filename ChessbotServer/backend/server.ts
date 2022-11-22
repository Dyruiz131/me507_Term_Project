import { Express } from "express";
import api from "./routes/api";

const express = require("express"); // 1. includes Express
const app: Express = express(); // 2. initializes Express
const cors = require("cors");
app.use(express.json()); // Enables JSON parsing
app.use(cors());

console.log("Backend running");

// Routes go here
app.use("/", api); // 3. adds the recipe endpoints

app.listen(3001); // 4. runs Express

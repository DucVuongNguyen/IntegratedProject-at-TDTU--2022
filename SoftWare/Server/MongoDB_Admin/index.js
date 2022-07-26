const express = require("express");
const cors = require("cors");
const app = express();
var bodyParser = require("body-parser");
const morgan = require("morgan");
const dotenv = require("dotenv");
const API_Route = require("./API_routes/api_routes");
const port = 8000;
limit_json_data = 50;
app.use(bodyParser.json({ limit: `${limit_json_data}mb` }));
app.use(express.urlencoded({ extended: true }));
app.use(cors());
app.use(morgan("common"));


API_Route(app);
app.get('/',(req,res)=>{
    res.send('Chào mừng bạn đến với Vhome');
})
app.listen(port, () => {
    console.log(`Server is running at port ${port}`);
})



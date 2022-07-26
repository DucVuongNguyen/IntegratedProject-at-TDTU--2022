const router = require("express").Router();
const api_Switch = require("../API_Controllers/api_Switch");
const api_Check = require("../API_Controllers/api_Check");
const api_Device = require("../API_Controllers/api_Device");

const API_Route = (app) => {
    // switch
    router.post('/changeKey', api_Switch.changeKey);
    router.post('/resetKey', api_Switch.resetKey);
    router.post('/sendData', api_Switch.sendData);
    router.post('/readData', api_Switch.readData);
    router.post('/sendData_KeySecurity', api_Switch.sendData_KeySecurity);
    router.post('/readData_KeySecurity', api_Switch.readData_KeySecurity);
    router.post('/keepAlive', api_Switch.keep_Alive);
    router.post('/setSchedule', api_Switch.setSchedule);
    router.post('/checkAlive', api_Switch.checkAlive);
    router.post('/loadSchedule', api_Switch.loadSchedule);
    router.post('/updateKey', api_Switch.updateKey);

    // user
    router.post('/checkUser', api_Check.checkUser);
    router.post('/checkDevice', api_Check.checkDevice);
    //add
    router.post('/addDevice', api_Device.addDevice);
    router.post('/deleteDevice', api_Device.deleteDevice);

    return app.use('/api', router)

}
module.exports = API_Route;
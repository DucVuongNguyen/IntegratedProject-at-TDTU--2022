const { MongoClient } = require("mongodb");
const dotenv = require("dotenv");
dotenv.config();

let updateKey = async (req, res) => {
    const client = new MongoClient(process.env.MONGODB_URL);

    try {

        if (!req.body.UserName || !req.body.Password || !req.body.NameDevice || !req.body.NewKey) {
            return res.status(200).json({
                message: `Thông tin không để trống`,
                isError: 1
            });
        }
        let NameDevice = req.body.NameDevice;
        let NewKey = req.body.NewKey;
        let UserName = req.body.UserName;
        let Password = req.body.Password;
        // console.log(`NamDoc: ${NameDevice}`);
        // console.log(`NewKey: ${NewKey}`);
        // console.log(`OldKey: ${OldKey}`);
        let db = `Devices_Manager`;
        let coll = `Devices_`;

        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ NameDevice: NameDevice, Key: NewKey });
        let Response_ = result;
        if (Response_) {
            let db = `ManagerAccounts`;
            let coll = `Users`;
            let result = await client.db(`${db}`).collection(`${coll}`).findOne({ UserName: UserName, Password: Password });
            let response_ = result;
            if (response_) {
                let DevicesArr = response_.Devices;
                console.log(DevicesArr)
                let DevicesUpdate = DevicesArr.map((device) => {
                    if (device.NameDevice === NameDevice) {
                        device.Key = NewKey;
                    }
                    return device

                })
                console.log(DevicesUpdate)
                let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ UserName: UserName, Password: Password }, { $set: { Devices: DevicesUpdate } });
                let Response_ = result;
                if (Response_) {
                    return res.status(200).json({
                        message: `Đã cập nhật Key cho thiết bị`,
                        isError: 0
                    });
                }
                else {
                    return res.status(200).json({
                        message: `Quá trình xảy ra lỗi`,
                        isError: 1
                    });
                }
            }
            else {
                return res.status(200).json({
                    message: `Thông tin không chính xác!`,
                    isError: 1
                });
            }
        } else {
            return res.status(200).json({
                message: `Key không chínnh xác!`,
                isError: 1
            });
        }
    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình kết nối xảy ra lỗi! Vui lòng thực hiện lại.`,
            isError: 1
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }
}

let changeKey = async (req, res) => {
    const client = new MongoClient(process.env.MONGODB_URL);

    try {

        if (!req.body.NameDevice || !req.body.OldKey || !req.body.NewKey) {
            return res.status(200).json({
                message: `Thông tin không để trống`,
                isError: 1
            });
        }
        let NameDevice = req.body.NameDevice;
        let OldKey = req.body.OldKey;
        let NewKey = req.body.NewKey;
        // console.log(`NamDoc: ${NameDevice}`);
        // console.log(`NewKey: ${NewKey}`);
        // console.log(`OldKey: ${OldKey}`);
        let db = `Devices_Manager`;
        let coll = `Devices_`;

        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ NameDevice: NameDevice, Key: OldKey });
        let Response_ = result;
        if (Response_) {
            let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ NameDevice: NameDevice }, { $set: { Key: NewKey, TimeModify: Date() } });
            // console.log(`${result.matchedCount} document(s) matched the query criteria.`);
            // console.log(`${result.modifiedCount} document(s) was/were updated.`);
            return res.status(200).json({
                message: `Thiết bị ${NameDevice} cật nhật mật khẩu thành công`,
                isError: 0
            });

        }
        else {
            return res.status(200).json({
                message: `Thông tin thiết bị không chính xác`,
                isError: 1
            });
        }
    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình kết nối xảy ra lỗi! Vui lòng thực hiện lại.`,
            isError: 1
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }
}

let resetKey = async (req, res) => {

    const client = new MongoClient(process.env.MONGODB_URL);
    try {
        if (!req.body.KeySecurity || !req.body.NameDevice) {
            return res.status(200).json({
                message: `Thông tin không để trống`,
                isError: 1
            });
        }
        let NameDevice = req.body.NameDevice;
        let KeySecurity = req.body.KeySecurity;
        // console.log(`NamDoc: ${NameDevice}`);
        // console.log(`KeySecurity : ${KeySecurity}`);
        let db = `Devices_Manager`;
        let coll = `Devices_`;

        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ NameDevice: NameDevice, KeySecurity: KeySecurity });
        let Response_ = result;
        if (Response_) {


            let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ NameDevice: NameDevice }, { $set: { Key: "admin", TimeModify: Date() } });
            // console.log(`${result.matchedCount} document(s) matched the query criteria.`);
            // console.log(`${result.modifiedCount} document(s) was/were updated.`);
            return res.status(200).json({
                message: `Thiết bị ${NameDevice} reset thành công`,
                isError: 0
            });

        }
        else {
            return res.status(200).json({
                message: `Thông tin thiết bị không chính xác`,
                isError: 1
            });
        }
    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình kết nối xảy ra lỗi! Vui lòng thực hiện lại.`,
            isError: 1
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }




}

let sendData = async (req, res) => {
    const client = new MongoClient(process.env.MONGODB_URL);

    try {

        if (!req.body.NameDevice || !req.body.Key || !req.body.Status) {
            return res.status(200).json({
                message: `Thông tin không để trống.`,
                isError: 1
            });
        }
        let NameDevice = req.body.NameDevice;
        let Key = req.body.Key;
        let Status = req.body.Status;
        let sample = { Status: Status, TimeModify: Date() }
        // console.log(`NamDoc: ${NameDevice}`);
        // console.log(`Key: ${Key}`);
        let db = `Devices_Manager`;
        let coll = `Devices_`;
        let TimeON_s = 0, TimeON_e = 0, TimeOFF_s = 0, TimeOFF_e = 0;
        let today = new Date();
        let day = today.getDate();
        let month = today.getMonth() + 1;
        let year = today.getFullYear();
        let hours_ = today.getHours();
        let minutes_ = today.getMinutes();
        let timer_ = hours_ * 60 + minutes_;
        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ NameDevice: NameDevice, Key: Key });
        let Response_ = result;
        if (Response_) {

            // console.log(result.NameDevice);
            db = Response_.Type;
            coll = Response_.NameDevice;
            if (Response_.set_Schedule.Enable === "1") {
                TimeON_s = Number(Response_.set_Schedule.setting.TimeON_s.TimeON_hours_s) * 60 + Number(Response_.set_Schedule.setting.TimeON_s.TimeON_minutes_s);
                TimeON_e = Number(Response_.set_Schedule.setting.TimeON_e.TimeON_hours_e) * 60 + Number(Response_.set_Schedule.setting.TimeON_e.TimeON_minutes_e);
                TimeOFF_s = Number(Response_.set_Schedule.setting.TimeOFF_s.TimeOFF_hours_s) * 60 + Number(Response_.set_Schedule.setting.TimeOFF_s.TimeOFF_minutes_s);
                TimeOFF_e = Number(Response_.set_Schedule.setting.TimeOFF_e.TimeOFF_hours_e) * 60 + Number(Response_.set_Schedule.setting.TimeOFF_e.TimeOFF_minutes_e);
                // console.log(`hours_ : ${hours_}`);
                // console.log(`minutes_: ${minutes_}`);
                // console.log(`timer_: ${timer_}`);
                // console.log(`TimeON_s: ${TimeON_s}`);
                // console.log(`TimeON_e: ${TimeON_e}`);
                // console.log(`TimeOFF_s: ${TimeOFF_s}`);
                // console.log(`TimeOFF_e: ${TimeOFF_e}`);
                if ((timer_ >= TimeON_s) && (timer_ <= TimeON_e)) {
                    sample = { Status: 1, TimeModify: Date() }
                    let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ Day: day, Month: month, Year: year }, { $push: { samples: sample } }, { upsert: true });
                    return res.status(200).json({
                        message: `Thiết bị ${NameDevice} đọc dữ liệu thành công`,
                        isError: 0
                    });
                }
                if ((timer_ >= TimeOFF_s) && (timer_ <= TimeOFF_e)) {
                    sample = { Status: 0, TimeModify: Date() }
                    let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ Day: day, Month: month, Year: year }, { $push: { samples: sample } }, { upsert: true });
                    return res.status(200).json({
                        message: `Thiết bị ${NameDevice} đọc dữ liệu thành công`,
                        isError: 0
                    });
                }

            }
            sample = { Status: Status, TimeModify: Date() }
            let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ Day: day, Month: month, Year: year }, { $push: { samples: sample } }, { upsert: true });
            // console.log(`${result.insertedCount} new listing(s) created with the following id(s):`);
            // console.log(result.insertedIds);
            return res.status(200).json({
                message: `Thiết bị ${NameDevice} thêm dữ liệu thành công`,
                isError: 0
            });
        }
        else {
            return res.status(200).json({
                message: `Thông tin thiết bị đã được thay đổi`,
                isError: 2,
            });
        }
    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình kết nối xảy ra lỗi! Vui lòng thực hiện lại.`,
            isError: 2
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }
}


let readData = async (req, res) => {
    const client = new MongoClient(process.env.MONGODB_URL);

    try {

        if (!req.body.NameDevice || !req.body.Key) {
            return res.status(200).json({
                message: `Thông tin không để trống!`,
                isError: 1
            });
        }
        let NameDevice = req.body.NameDevice;
        let Key = req.body.Key;
        // console.log(`NamDoc: ${NameDevice}`);
        // console.log(`Key: ${Key}`);
        let db = `Devices_Manager`;
        let coll = `Devices_`;
        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ NameDevice: NameDevice, Key: Key });
        // console.log(result);
        let Response_ = result;
        if (Response_) {
            db = Response_.Type;
            coll = Response_.NameDevice;
            let today = new Date();
            let day = today.getDate();
            let month = today.getMonth() + 1;
            let year = today.getFullYear();
            let result = await client.db(`${db}`).collection(`${coll}`).findOne({ Day: day, Month: month, Year: year });
            let response_ = result;

            if (response_) {
                let obj = response_.samples;
                let lastItem = Object.keys(obj).length - 1;
                if (lastItem < 0) {
                    lastItem = 0;
                }

                // console.log(obj[0].Status)
                return res.status(200).json({
                    message: `Thiết bị ${NameDevice} đọc dữ liệu thành công`,
                    isError: 0,
                    DataResult: obj[lastItem].Status
                });
            }
            else {
                let today = new Date();
                let yesterday = new Date(today - 1000 * 60 * 60 * 24);
                let day = yesterday.getDate();
                let month = yesterday.getMonth() + 1;
                let year = yesterday.getFullYear();
                let result = await client.db(`${db}`).collection(`${coll}`).findOne({ Day: day, Month: month, Year: year });
                let Response_ = result;
                if (Response_) {

                    let obj = Response_.samples;
                    let lastItem = Object.keys(obj).length - 1;
                    if (lastItem < 0) {
                        lastItem = 0;
                    }
                    sample = { Status: obj[lastItem].Status, TimeModify: Date() }
                    let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ Day: today.getDate(), Month: today.getMonth() + 1, Year: today.getFullYear() }, { $push: { samples: sample } }, { upsert: true });
                    // console.log(obj[0].Status)
                    return res.status(200).json({
                        message: `Thiết bị ${NameDevice} đọc dữ liệu thành công`,
                        isError: 0,
                        DataResult: obj[lastItem].Status
                    });
                }
                else {
                    return res.status(200).json({
                        message: `Quá trình xảy ra lỗi`,
                        isError: 1,
                    });
                }

            }


        }
        else {
            return res.status(200).json({
                message: `Thông tin thiết bị đã được thay đổi`,
                isError: 2,
            });
        }
    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình kết nối xảy ra lỗi! Vui lòng thực hiện lại.`,
            isError: 1
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }
}

let sendData_KeySecurity = async (req, res) => {
    const client = new MongoClient(process.env.MONGODB_URL);

    try {

        if (!req.body.NameDevice || !req.body.KeySecurity || !req.body.Status) {
            return res.status(200).json({
                message: `Thông tin không để trống.`,
                isError: 1
            });
        }
        let NameDevice = req.body.NameDevice;
        let KeySecurity = req.body.KeySecurity;
        let Status = req.body.Status;
        // console.log(`NamDoc: ${NameDevice}`);
        // console.log(`KeySecurity: ${KeySecurity}`);
        let db = `Devices_Manager`;
        let coll = `Devices_`;
        let TimeON_s = 0, TimeON_e = 0, TimeOFF_s = 0, TimeOFF_e = 0;
        let today = new Date();
        let day = today.getDate();
        let month = today.getMonth() + 1;
        let year = today.getFullYear();
        let hours_ = today.getHours();
        let minutes_ = today.getMinutes();
        let timer_ = hours_ * 60 + minutes_;
        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ NameDevice: NameDevice, KeySecurity: KeySecurity });
        let Response_ = result;
        if (Response_) {

            // console.log(result.NameDevice);
            db = Response_.Type;
            coll = Response_.NameDevice;
            if (Response_.set_Schedule.Enable === "1") {
                TimeON_s = Number(Response_.set_Schedule.setting.TimeON_s.TimeON_hours_s) * 60 + Number(Response_.set_Schedule.setting.TimeON_s.TimeON_minutes_s);
                TimeON_e = Number(Response_.set_Schedule.setting.TimeON_e.TimeON_hours_e) * 60 + Number(Response_.set_Schedule.setting.TimeON_e.TimeON_minutes_e);
                TimeOFF_s = Number(Response_.set_Schedule.setting.TimeOFF_s.TimeOFF_hours_s) * 60 + Number(Response_.set_Schedule.setting.TimeOFF_s.TimeOFF_minutes_s);
                TimeOFF_e = Number(Response_.set_Schedule.setting.TimeOFF_e.TimeOFF_hours_e) * 60 + Number(Response_.set_Schedule.setting.TimeOFF_e.TimeOFF_minutes_e);
                // console.log(`hours_ : ${hours_}`);
                // console.log(`minutes_: ${minutes_}`);
                // console.log(`timer_: ${timer_}`);
                // console.log(`TimeON_s: ${TimeON_s}`);
                // console.log(`TimeON_e: ${TimeON_e}`);
                // console.log(`TimeOFF_s: ${TimeOFF_s}`);
                // console.log(`TimeOFF_e: ${TimeOFF_e}`);
                if ((timer_ >= TimeON_s) && (timer_ <= TimeON_e)) {
                    sample = { Status: 1, TimeModify: Date() }
                    let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ Day: day, Month: month, Year: year }, { $push: { samples: sample } }, { upsert: true });
                    return res.status(200).json({
                        message: `Thiết bị ${NameDevice} đọc dữ liệu thành công`,
                    });
                }
                if ((timer_ >= TimeOFF_s) && (timer_ <= TimeOFF_e)) {
                    sample = { Status: 0, TimeModify: Date() }
                    let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ Day: day, Month: month, Year: year }, { $push: { samples: sample } }, { upsert: true });
                    return res.status(200).json({
                        message: `Thiết bị ${NameDevice} đọc dữ liệu thành công`,
                    });
                }

            }
            sample = { Status: Status, TimeModify: Date().toUTCString }
            let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ Day: day, Month: month, Year: year }, { $push: { samples: sample } }, { upsert: true });
            // console.log(`${result.insertedCount} new listing(s) created with the following id(s):`);
            // console.log(result.insertedIds);
            return res.status(200).json({
                message: `Thiết bị ${NameDevice} thêm dữ liệu thành công`,
                isError: 0
            });
        }
        else {
            return res.status(200).json({
                message: `Thông tin thiết bị đã được thay đổi`,
                isError: 2,
            });
        }
    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình kết nối xảy ra lỗi! Vui lòng thực hiện lại.`,
            isError: 1
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }
}


let readData_KeySecurity = async (req, res) => {
    const client = new MongoClient(process.env.MONGODB_URL);

    try {

        if (!req.body.NameDevice || !req.body.KeySecurity) {
            return res.status(200).json({
                message: `Thông tin không để trống!`,
                isError: 1
            });
        }
        let NameDevice = req.body.NameDevice;
        let KeySecurity = req.body.KeySecurity;
        // console.log(`NamDoc: ${NameDevice}`);
        // console.log(`KeySecurity: ${KeySecurity}`);
        let db = `Devices_Manager`;
        let coll = `Devices_`;
        let TimeON_s = 0, TimeON_e = 0, TimeOFF_s = 0, TimeOFF_e = 0;
        let today = new Date();
        let day = today.getDate();
        let month = today.getMonth() + 1;
        let year = today.getFullYear();
        let hours_ = today.getHours();
        let minutes_ = today.getMinutes();
        let timer_ = hours_ * 60 + minutes_;
        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ NameDevice: NameDevice, KeySecurity: KeySecurity });
        let Response_ = result;
        // console.log(result);  
        if (Response_) {
            db = Response_.Type;
            coll = Response_.NameDevice;
            let today = new Date();
            let day = today.getDate();
            let month = today.getMonth() + 1;
            let year = today.getFullYear();
            result = await client.db(`${db}`).collection(`${coll}`).findOne({ Day: day, Month: month, Year: year });
            let obj = result.samples;
            let lastItem = Object.keys(obj).length - 1;
            if (lastItem < 0) {
                lastItem = 0;
            }
            if (Number(Response_.set_Schedule.Enable) === 0) {
                // console.log(obj[0].Status)
                return res.status(200).json({
                    message: `Thiết bị ${NameDevice} đọc dữ liệu thành công`,
                    isError: 0,
                    DataResult: obj[lastItem].Status
                });
            }
            if (Number(Response_.set_Schedule.Enable) === 1) {
                let hasSchedule = 0;
                let sample = { Status: 0, TimeModify: Date() }
                TimeON_s = Number(Response_.set_Schedule.setting.TimeON_s.TimeON_hours_s) * 60 + Number(Response_.set_Schedule.setting.TimeON_s.TimeON_minutes_s);
                TimeON_e = Number(Response_.set_Schedule.setting.TimeON_e.TimeON_hours_e) * 60 + Number(Response_.set_Schedule.setting.TimeON_e.TimeON_minutes_e);
                TimeOFF_s = Number(Response_.set_Schedule.setting.TimeOFF_s.TimeOFF_hours_s) * 60 + Number(Response_.set_Schedule.setting.TimeOFF_s.TimeOFF_minutes_s);
                TimeOFF_e = Number(Response_.set_Schedule.setting.TimeOFF_e.TimeOFF_hours_e) * 60 + Number(Response_.set_Schedule.setting.TimeOFF_e.TimeOFF_minutes_e);
                if ((timer_ >= TimeON_s) && (timer_ <= TimeON_e)) {
                    hasSchedule = 1;
                    sample = { Status: 1, TimeModify: Date() }
                }
                if ((timer_ >= TimeOFF_s) && (timer_ <= TimeOFF_e)) {
                    hasSchedule = 1;
                    sample = { Status: 0, TimeModify: Date() }
                }

                if (Number(obj[lastItem].Status) == Number(sample.Status)) {
                    return res.status(200).json({
                        message: `Thiết bị ${NameDevice} đọc dữ liệu thành công`,
                        isError: 0,
                        DataResult: obj[lastItem].Status
                    });

                }
                if (Number(obj[lastItem].Status) !== Number(sample.Status) && hasSchedule) {
                    let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ Day: day, Month: month, Year: year }, { $push: { samples: sample } }, { upsert: true });
                    if (result) {
                        return res.status(200).json({
                            message: `Thiết bị ${NameDevice} đọc dữ liệu thành công`,
                            isError: 0,
                            DataResult: obj[lastItem].Status
                        });
                    }
                    else {
                        return res.status(200).json({
                            message: `Quá trình kết nối xảy ra lỗi! Vui lòng thực hiện lại.`,
                            isError: 1
                        });
                    }

                }
                else {
                    return res.status(200).json({
                        message: `Thiết bị ${NameDevice} đọc dữ liệu thành công`,
                        isError: 0,
                        DataResult: obj[lastItem].Status
                    });
                }
            }

        }
        else {
            return res.status(200).json({
                message: `Thông tin thiết bị đã được thay đổi`,
                isError: 2,
            });
        }
    }
    catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình kết nối xảy ra lỗi! Vui lòng thực hiện lại.`,
            isError: 1
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }
}

let keep_Alive = async (req, res) => {

    const client = new MongoClient(process.env.MONGODB_URL);
    try {
        if (!req.body.KeySecurity || !req.body.NameDevice || !req.body.RSSI) {
            return res.status(200).json({
                message: `Yêu cầu tên đăng nhập và mật khẩu để truy cập Database.`,
                isError: 1
            });
        }
        let NameDevice = req.body.NameDevice;
        let KeySecurity = req.body.KeySecurity;
        let RSSI = req.body.RSSI;
        // console.log(`NamDoc: ${NameDevice}`);
        // console.log(`KeySecurity : ${KeySecurity}`);
        let db = `Devices_Manager`;
        let coll = `Devices_`;
        let time_now = Date();
        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ NameDevice: NameDevice, KeySecurity: KeySecurity });
        let Response_ = result;
        if (Response_) {


            let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ NameDevice: NameDevice, KeySecurity: KeySecurity }, {
                $set: {

                    keep_Alive: {
                        TimeAlive: time_now,
                        RSSI: RSSI
                    }


                }
            });
            // console.log(`${result.matchedCount} document(s) matched the query criteria.`);
            // console.log(`${result.modifiedCount} document(s) was/were updated.`);
            return res.status(200).json({
                message: `Thiết bị ${NameDevice} đang giữ kết nối với server`,
                isError: 0
            });

        }
        else {
            return res.status(200).json({
                message: `Thông tin thiết bị đã được thay đổi`,
                isError: 2,
            });
        }
    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình kết nối xảy ra lỗi! Vui lòng thực hiện lại.`,
            isError: 1
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }
}

let setSchedule = async (req, res) => {

    const client = new MongoClient(process.env.MONGODB_URL);
    try {
        if (!req.body.NameDevice || !req.body.Key) {
            return res.status(200).json({
                message: `Yêu cầu tên đăng nhập và mật khẩu để truy cập Database.`,
                isError: 1
            });
        }
        let NameDevice = req.body.NameDevice;
        let Key = req.body.Key;
        // console.log(`NamDoc: ${NameDevice}`);
        // console.log(`Key : ${Key}`);
        let db = `Devices_Manager`;
        let coll = `Devices_`;
        let req_BD = false;
        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ NameDevice: NameDevice, Key: Key });
        let Response_ = result;
        if (Response_) {
            let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ NameDevice: NameDevice }, {
                $set: {
                    set_Schedule: {
                        Enable: req.body.EnableSchedule,
                        setting: {

                            TimeON_s: {
                                TimeON_hours_s: req.body.TimeON_hours_s,
                                TimeON_minutes_s: req.body.TimeON_minutes_s,
                            },
                            TimeON_e: {
                                TimeON_hours_e: req.body.TimeON_hours_e,
                                TimeON_minutes_e: req.body.TimeON_minutes_e,
                            },
                            TimeOFF_s: {
                                TimeOFF_hours_s: req.body.TimeOFF_hours_s,
                                TimeOFF_minutes_s: req.body.TimeOFF_minutes_s,
                            },
                            TimeOFF_e: {
                                TimeOFF_hours_e: req.body.TimeOFF_hours_e,
                                TimeOFF_minutes_e: req.body.TimeOFF_minutes_e,
                            },

                        },
                        TimeModify: Date()
                    }
                }
            });
            // console.log(`${result.insertedCount} new listing(s) created with the following id(s):`);
            // console.log(result.insertedIds);
            return res.status(200).json({
                message: `Thiết bị ${NameDevice} thêm dữ liệu thành công`,
                isError: 0
            });

        }
        else {
            return res.status(200).json({
                message: `Thông tin thiết bị đã được thay đổi`,
                isError: 2,
            });
        }
    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình xảy ra lỗi! Vui lòng thực hiện lại.`,
            isError: 1
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }
}


let checkAlive = async (req, res) => {

    const client = new MongoClient(process.env.MONGODB_URL);

    try {

        if (!req.body.NameDevice || !req.body.Key) {
            return res.status(200).json({
                message: `Thông tin không để trống!`,
                isError: 1
            });
        }
        let NameDevice = req.body.NameDevice;
        let Key = req.body.Key;
        // console.log(`NamDoc: ${NameDevice}`);
        // console.log(`Key: ${Key}`);
        let db = `Devices_Manager`;
        let coll = `Devices_`;
        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ NameDevice: NameDevice, Key: Key });
        // console.log(result);
        let Response_ = result;
        if (Response_) {
            let time_past = new Date(Response_.keep_Alive.TimeAlive);
            let time_now = new Date();
            // console.log(`Response_.keep_Alive.TimeAlive ${Response_.keep_Alive.TimeAlive}`)
            // console.log(`time_past ${time_past}`)
            // console.log(`time_now ${time_now}`)

            let AmountOfTime = (time_now.getTime() - time_past.getTime()) / 1000 / 60;

            // console.log(AmountOfTime)
            if (AmountOfTime < 0.3) {
                return res.status(200).json({
                    message: `Cường độ tín hiệu: ${Response_.keep_Alive.RSSI}`,
                    isError: 0,
                });
            }
            else {
                return res.status(200).json({
                    message: `Thiết bị mất kết nối với Wifi!`,
                    isError: 1,
                });
            }
        }
        else {
            return res.status(200).json({
                message: `Thông tin thiết bị đã được thay đổi`,
                isError: 2,
            });
        }
    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình kết nối xảy ra lỗi! Vui lòng thực hiện lại.`,
            isError: 1
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }
}

let loadSchedule = async (req, res) => {

    const client = new MongoClient(process.env.MONGODB_URL);

    try {

        if (!req.body.NameDevice || !req.body.Key) {
            return res.status(200).json({
                message: `Thông tin không để trống!`,
                isError: 1
            });
        }
        let NameDevice = req.body.NameDevice;
        let Key = req.body.Key;
        // console.log(`NamDoc: ${NameDevice}`);
        // console.log(`Key: ${Key}`);
        let db = `Devices_Manager`;
        let coll = `Devices_`;
        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ NameDevice: NameDevice, Key: Key });
        // console.log(result);
        let Response_ = result;
        if (Response_) {
            console.log(Response_.set_Schedule.Enable)

            return res.status(200).json({
                message: `Quá trình hoàn tất `,
                isError: 0,
                DataResult: Response_.set_Schedule
            });

        }
        else {
            return res.status(200).json({
                message: `Thông tin thiết bị đã được thay đổi`,
                isError: 2,
            });
        }
    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình kết nối xảy ra lỗi! Vui lòng thực hiện lại.`,
            isError: 1
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }
}




module.exports = {
    changeKey, resetKey, sendData, readData, readData_KeySecurity, sendData_KeySecurity, keep_Alive, setSchedule, checkAlive, loadSchedule, updateKey
}
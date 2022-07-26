const { MongoClient } = require("mongodb");
const dotenv = require("dotenv");
dotenv.config();

let addDevice = async (req, res) => {
    const client = new MongoClient(process.env.MONGODB_URL);

    try {

        if (!req.body.UserName || !req.body.Password || !req.body.NameDevice || !req.body.Key || !req.body.Type) {
            return res.status(200).json({
                message: `Thông tin không để trống!`,
                isError: 1
            });
        }
        let UserName = req.body.UserName;
        let Password = req.body.Password;
        let NameDevice = req.body.NameDevice;
        let Key = req.body.Key;
        let Type = req.body.Type;
        let db = `ManagerAccounts`;
        let coll = `Users`;
        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ UserName: UserName, Password: Password });
        if (result) {
            // console.log(result.Devices)
            let DevicesArr = result.Devices;
            let hasDevice = false;
            let Arr = DevicesArr.map((device) => {
                if (device.NameDevice === NameDevice) {
                    hasDevice = true;
                }
            })
            console.log(hasDevice)
            if (!hasDevice) {
                let Device = {

                    NameDevice: NameDevice,
                    NameDeviceCustom: NameDevice,
                    Key: Key,
                    Type: Type,
                }

                let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ UserName: UserName, Password: Password }, { $push: { Devices: Device } });
                if (result) {
                    let result = await client.db(`${db}`).collection(`${coll}`).findOne({ UserName: UserName, Password: Password });

                    return res.status(200).json({
                        message: `Thiết bị đã được thêm`,
                        isError: 0,
                        user: result
                    });
                }
                else {
                    return res.status(200).json({
                        message: `Quá trình thêm thiết bị xảy ra lỗi`,
                        isError: 1
                    });
                }

            }
            else {
                return res.status(200).json({
                    message: `Thiết bị ${NameDevice} đã tồn tại!`,
                    isError: 1
                });

            }

        }
        else {
            return res.status(200).json({
                message: `Quá trình xảy ra lỗi!`,
                isError: 1
            });

        }

    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình xảy ra lỗi!`,
            isError: 0
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }
}

let deleteDevice = async (req, res) => {
    const client = new MongoClient(process.env.MONGODB_URL);
    try {

        if (!req.body.UserName || !req.body.Password || !req.body.NameDevice) {
            return res.status(200).json({
                message: `Thông tin không để trống!`,
                isError: 1
            });
        }
        let UserName = req.body.UserName;
        let Password = req.body.Password;
        let NameDevice = req.body.NameDevice;
        let db = `ManagerAccounts`;
        let coll = `Users`;
        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ UserName: UserName, Password: Password });
        let Response_ = result;
        if (Response_) {
            let DevicesArr = Response_.Devices;
            let Device = DevicesArr.filter((device) => {
                return device.NameDevice !== NameDevice;
            });


            let result = await client.db(`${db}`).collection(`${coll}`).updateOne({ UserName: UserName, Password: Password }, { $set: { Devices: Device } });
            if (result) {
                let result = await client.db(`${db}`).collection(`${coll}`).findOne({ UserName: UserName, Password: Password });

                return res.status(200).json({
                    message: `Thiết bị đã được xóa`,
                    isError: 0,
                    user: result
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
                message: `Quá trình xảy ra lỗi!`,
                isError: 1
            });

        }

    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình xảy ra lỗi!`,
            isError: 0
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }


}



module.exports = {
    addDevice, deleteDevice

}
const { MongoClient } = require("mongodb");
const dotenv = require("dotenv");
dotenv.config();
let checkUser = async (req, res) => {
    const client = new MongoClient(process.env.MONGODB_URL);

    try {

        if (!req.body.UserName || !req.body.Password) {
            return res.status(200).json({
                message: `Thông tin đăng nhập không bỏ trống!`,
                checkLogin: 0,
                isError: 1
            });
        }
        let UserName = req.body.UserName;
        let Password = req.body.Password;
        let db = `ManagerAccounts`;
        let coll = `Users`;
        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ UserName: UserName, Password: Password });
        if (result) {
            return res.status(200).json({
                message: `Đăng nhập thành công!`,
                checkLogin: 1,
                user: result,
                isError: 0

            });
        } else {
            return res.status(200).json({
                message: `Thông tin đăng nhập không hợp lệ!`,
                checkLogin: 0,
                isError: 1
            });
        }
    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Quá trình xảy ra lỗi! Vui lòng thực hiện lại!`,
            isError: 1
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }
}

let checkDevice = async (req, res) => {
    const client = new MongoClient(process.env.MONGODB_URL);

    try {

        if (!req.body.NameDevice || !req.body.Key) {
            return res.status(200).json({
                message: `Thông tin thiết bị không để trống!`,
                isError: 1
            });
        }
        let NameDevice = req.body.NameDevice;
        let Key = req.body.Key;
        // console.log(`NamDoc: ${NameDoc}`);
        // console.log(`NewPassword: ${NewPassword}`);
        // console.log(`OldPassword: ${OldPassword}`);
        let db = `Devices_Manager`;
        let coll = `Devices_`;
        let req_BD = false;

        await client.connect();
        let result = await client.db(`${db}`).collection(`${coll}`).findOne({ NameDevice: NameDevice });
        if (result.Key === Key) {
            // console.log(result);
            return res.status(200).json({
                message: ``,
                isError: 0,
                Type: result.Type
            });
        } else {
            console.log(`Thông tin thiết bị không chính xác!`);
            return res.status(200).json({
                message: `Thông tin thiết bị không chính xác!`,
                isError: 1
            });
        }
    } catch (e) {
        console.error(e);
        return res.status(200).json({
            message: `Thông tin thiết bị không chính xác!`,
            isError: 1
        });
    } finally {
        // Close the connection to the MongoDB cluster
        await client.close();
    }
}

module.exports = {
    checkUser, checkDevice
}
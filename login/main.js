const myId = document.getElementById("myId");
const myPasswd = document.getElementById("myPasswd");
const loginButton = document.getElementById("loginButton");
const errorText = document.getElementById("errorText");

errorText.innerText = "";

loginButton.addEventListener("click", () =>{
    console.log("myId:", myId.value);
    console.log("myPasswd:", myPasswd.value);

    if(myId.value === "" || myPasswd.value === "") {
        errorText.innerText = "請輸入帳號密碼";
    }else{
        errorText.innerText = "";
        let reqJson = {
            "Name": "checkIdPasswd",
            "Id": myId.value,
            "Passwd": myPasswd.value
        };
        checkIdPasswd(reqJson);
    }
});

function checkIdPasswd(reqJson) {
    let request = new XMLHttpRequest();
    request.open("POST", window.location.origin + "/data/checkIdPasswd", true);
    request.setRequestHeader("Content-Type", "application/json");

    request.send(JSON.stringify(reqJson));

    request.addEventListener("load", () => {
        const resJson = JSON.parse(request.responseText);
        if(resJson["Error"] == null) {
            window.location.href = "/home";
        } else {
            errorText.innerText = "帳號或密碼錯誤";
        }
    });
}
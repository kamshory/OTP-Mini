var ajax = {};
ajax.create = function () {
    if (typeof XMLHttpRequest !== 'undefined') {
        return new XMLHttpRequest();
    }
    var versions = [
        "MSXML2.XmlHttp.6.0",
        "MSXML2.XmlHttp.5.0",
        "MSXML2.XmlHttp.4.0",
        "MSXML2.XmlHttp.3.0",
        "MSXML2.XmlHttp.2.0",
        "Microsoft.XmlHttp"
    ];

    var xhr;
    for (var i = 0; i < versions.length; i++) {
        try {
            xhr = new ActiveXObject(versions[i]);
            break;
        } catch (e) 
        {
        }
    }
    return xhr;
};

ajax.send = function (url, callback, method, data, async) {
    if (async === undefined) {
        async = true;
    }
    var x = ajax.create();
    x.open(method, url, async);
    x.onreadystatechange = function () {
        if (x.readyState == 4) {
            callback(x.responseText)
        }
    };
    if (method == 'POST') {
        x.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    }
    x.send(data)
};

ajax.get = function (url, data, callback, async) {
    var query = [];
    for (var key in data) {
        if(data.hasOwnProperty(key))
        {
            query.push(encodeURIComponent(key) + '=' + encodeURIComponent(data[key]));
        }
    }
    ajax.send(url + (query.length ? '?' + query.join('&') : ''), callback, 'GET', null, async)
};

ajax.post = function (url, data, callback, async) {
    var query = [];
    for (var key in data) {
        if(data.hasOwnProperty(key))
        {
            query.push(encodeURIComponent(key) + '=' + encodeURIComponent(data[key]));
        }
    }
    ajax.send(url, callback, 'POST', query.join('&'), async)
};
function saveSubData()
{
    var ssid_name = document.querySelector('#ssid_name').value;
    var ssid_password = document.querySelector('#ssid_password').value;
    var ws_host = document.querySelector('#ws_host').value;
    var ws_port = document.querySelector('#ws_port').value;
    var ws_path = document.querySelector('#ws_path').value;
    var ws_username = document.querySelector('#ws_username').value;
    var ws_password = document.querySelector('#ws_password').value;
    var ws_topic = document.querySelector('#ws_topic').value;
    var enable = document.querySelector('#enable').value;
    ajax.post(
        'save-subscribtion', 
        {
            action:'save-subscribtion',
            ssid_name:ssid_name, 
            ssid_password:ssid_password, 
            ws_host:ws_host, 
            ws_port:ws_port, 
            ws_path:ws_path, 
            ws_username:ws_username, 
            ws_password:ws_password,
            ws_topic:ws_topic,
            enable:enable
        },
        function(response){

        },
        true
    );
    return false;
}
function loadSubData()
{
    ajax.get(
        'subscribtion-configuration.json', 
        {
            
        },
        function(response){
            try{
                var data = JSON.parse(response);
                document.querySelector('#ssid_name').value = data.ssid_name;
                document.querySelector('#ssid_password').value = data.ssid_password;
                document.querySelector('#ws_host').value = data.ws_host;
                document.querySelector('#ws_port').value = data.ws_port;
                document.querySelector('#ws_path').value = data.ws_path;
                document.querySelector('#ws_username').value = data.ws_username;
                document.querySelector('#ws_password').value = data.ws_password;
                document.querySelector('#ws_topic').value = data.ws_topic;
                document.querySelector('#enable').value = data.enable;
            }
            catch(ex)
            {

            }
            
        },
        true
    );
}
function loadAPData()
{
    ajax.get(
        'ap-configuration.json', 
        {
            
        },
        function(response){
            try{
                var data = JSON.parse(response);
                document.querySelector('#ssid_name').value = data.ssid_name;
                document.querySelector('#ssid_password').value = data.ssid_password;    
                document.querySelector('#ip').value = data.ip;    
                document.querySelector('#gateway').value = data.gateway;    
                document.querySelector('#subnet').value = data.subnet;    
                document.querySelector('#hidden').value = data.hidden;    
            }
            catch(ex)
            {

            }
        },
        true
    );
}
function saveAPData()
{
    var ssid_name = document.querySelector('#ssid_name').value;
    var ssid_password = document.querySelector('#ssid_password').value;
    var ip = document.querySelector('#ip').value;
    var gateway = document.querySelector('#gateway').value;
    var subnet = document.querySelector('#subnet').value;
    var hidden = document.querySelector('#hidden').value;
    ajax.post(
        'save-ap', 
        {
            action:'save-ap',
            ssid_name:ssid_name, 
            ssid_password:ssid_password,
            ip:ip,
            gateway:gateway,
            subnet:subnet,
            hidden:hidden
        },
        function(response){

        },
        true
    );
    return false;
}
window.onload = function()
{
    var uri = window.location.toString();
    if(uri.indexOf('ap-configuration.html') > -1)
    {
        loadAPData();
    }
    if(uri.indexOf('subscribtion-configuration.html') > -1)
    {
        loadSubData();
    }
    const ipAdd = document.querySelectorAll('input[type="ipaddress"]');
    if(ipAdd.length)
    {
        for(var i = 0; i < ipAdd.length; i++)
        {
            ipAdd[i].addEventListener('keyup', function(e){
                handleIP(e);
            });
            ipAdd[i].addEventListener('change', function(e){
                handleIP(e);
            });
        }
    }
}
function handleIP(e)
{
    var obj = e.target;
    var value = obj.value;
    if(isValidIP(value))
    {
        obj.classList.remove('invalid-ip');
    }
    else
    {
        obj.classList.remove('invalid-ip');
        obj.classList.add('invalid-ip');
    }
}
function isValidIP(ip)
{
    if(ip.length == 0)
    {
        return true;
    }
    var arr = ip.split('.');
    if(arr.length != 4)
    {
        return false;
    }
    for(var i in arr)
    {
        if(isNaN(parseInt(arr[i])))
        {
            return false;
        }
        if(arr[i] < 0 || arr[i] > 255)
        {
            return false;
        }
    }
    return true;
}
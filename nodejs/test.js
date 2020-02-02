const http = require("http");

const hostname = "127.0.0.1";
const port = 3000;

const server = http.createServer((req, res) => {
  // 
  // -- require authentication
  var header=req.headers['authorization']||'',        // get the header
    token=header.split(/\s+/).pop()||'',            // and the encoded auth token
    auth=new Buffer.from(token, 'base64').toString(),    // convert from base64
    parts=auth.split(/:/),                          // split on colon
    username=parts[0],
    password=parts[1];

    // -- 


  var fs = require("fs");


  if(!fs.existsSync("./config.json")) {
    console.log("file does not exist");
  }
  var configString = fs.readFileSync("config.json");
  var config = JSON.parse( configString );

  // --345678

  if ((username!=config.username) || (password!=config.password)) {
    res.statusCode = 401;
    res.setHeader("WWW-Authenticate", "Basic realm=\"insert realm\"");
    res.setHeader("Content-Type", "text/plain");
    res.write('username is "'+username+'" and password is "'+password+'"');
    res.write("Hey, who are you!!");
    res.end();
    return;
  }
  //
  // -- authenticated, process url
  res.statusCode = 200;
  res.setHeader("Content-Type", "text/plain");
  var url = req.url;
  var urlSegments = req.url.split("/");
  res.write("urlSegments: " + JSON.stringify( urlSegments ))
  if (urlSegments.length===0) {
     res.write("invalid endpoint");
     res.end();
     return;
  }
  if(urlSegments[1]==="create") {
      if (urlSegments.length<2) {
        res.end("Argument Exception, please include vmName in /create/vmName");
        return;
      }
      res.write("\ncreating vm [" + urlSegments[2] + "]... , " + myexec() );
      res.end(); 
   } else if(urlSegments[1] ==="delete") {
    if (urlSegments.length<2) {
      res.end("Argument Exception, please include vmName in /delete/vmName");
      return;
    }
    res.write("\ndeleting vm [" + urlSegments[2] + "]... , " + myexec() );
    res.end(); 
 } else {
    res.write("\nCommand not recognized. Use:"); 
    res.write("\n\t/create/vmName"); 
    res.write("\n\t/delete/vmName"); 
    res.end(); 
   }
});

server.listen(port, hostname, () => {
  console.log(`Server running at http://${hostname}:${port}/`);
});

const execSync = require("child_process").execSync;

function myexec() {
    return execSync("mybat.bat", (error, stdout, stderr) => {
        if (error) {
            console.log(`error: ${error.message}`);
            return "error";
        }
        if (stderr) {
            console.log(`stderr: ${stderr}`);
            return "stderr";
        }
        //console.log(`stdout: ${stdout}`);
    });
}
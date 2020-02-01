const http = require('http');

const hostname = '127.0.0.1';
const port = 3000;

const server = http.createServer((req, res) => {
  res.statusCode = 200;
  res.setHeader('Content-Type', 'text/plain');
  res.end('Hello World, ' + myexec() );
  
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
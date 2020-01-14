import requests 
import http.server
import socketserver
import serial
import time
# Initialize serial streams
# serDrink = serial.Serial('COM3', baudrate=9600)
# if not serDrink.isOpen():
#     serDrink.open()

time.sleep(3)

# Server states
camSecurity = True
changeDetected = False

liquids = [
    {
        "name": "water",
        "volume": 2000,
    },
    {
        "name": "cola",
        "volume": 1000,
    },
    {
        "name": "fanta",
        "volume": 1000,
    }
]

class RequestHandlerMMI(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        print("Received a get request")
        global req, changeDetected, camSecurity
        req = self.requestline
        req = req[5: int(len(req)-9)]
        req = req.strip('/')
        reqParams = req.split('/')

        print(req)

        # Liquid requests
        if reqParams[0] == 'liquid':
            print("Got liquid request")

            # Get name and volume
            if reqParams[1] == 'get':
                for liquid in liquids:
                    if(reqParams[2] == liquid["name"]):
                        self.send_http_response(str(liquid))
                        return 200
                
                self.send_http_response("turned off")
                return "Liquid not found", 404

        # Security requests
        if reqParams[0] == 'securitymodule':
            print("Got security request")
            # Check if something was detected
            if reqParams[1] == 'detected':
                
                if camSecurity is True:
                    if changeDetected is True:
                        changeDetected = False
                        self.send_http_response("detected")
                    else:
                        self.send_http_response("notdetected")
                    
                return 200

    def do_POST(self):
        print("Received a post request")
        global req, serDrink, camSecurity, changeDetected
        req = self.requestline
        req = req[5: int(len(req)-9)]
        req = req.strip('/')
        reqParams = req.split('/')
        print(req)
        print(reqParams)

        # Beverage request
        if reqParams[0] == 'beveragemachine':
            print("Got beverage request")
            if reqParams[1] == 'makedrink':
                if len(reqParams) == 4:
                    # Send command to arduino
                    firstParam = reqParams[2]
                    secondParam = reqParams[3]

                    vol1 = str(int(firstParam)).encode('utf-8')
                    vol2 = str(int(secondParam)).encode('utf-8')
                    
                    print(vol1, vol2)

                    serDrink.write(vol1)
                    time.sleep(1)
                    serDrink.write(vol2)

                    self.send_http_response(str("Started drink"))


        if reqParams[0] == 'securitymodule':
            print("Got security request")

            # Turn security on or off
            if reqParams[1] == 'securityonoff':
               

                option = reqParams[2]
                if option == '1':
                    camSecurity = True
                elif option == '0':
                    camSecurity = False
                else:
                    print("Some strange case")
                
                if camSecurity is True:
                    print("Security activated")
                    self.send_http_response("Security activated")
                    return 200
                elif camSecurity is False:
                    print("Security deactivated")
                    self.send_http_response("Security deactivated")
                    return 200

                print("Error setting security")
                self.send_http_response("Error setting security")
                return 404

            # Notify of intruder
            if reqParams[1] == 'intruder':
                print("Notified of intruder")
                changeDetected = True
                self.send_http_response("Server was notified")
                return 200

            

    def send_http_response(self, message):
        rmesage = bytes(str(message), 'utf-8')
        self.send_response(200)
        self.send_header("Content-Type", "textplain")
        self.send_header("Content-Length", len(rmesage))
        self.end_headers()
        self.wfile.write(rmesage)
        print("Sent string {}".format(rmesage))
        
serverAdress = ('127.0.0.1', 8080)
httpServer = http.server.HTTPServer(serverAdress, RequestHandlerMMI)
print("Starting server")
httpServer.serve_forever()

    
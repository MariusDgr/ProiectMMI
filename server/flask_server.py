from flask import Flask, jsonify
from flask_restful import Api, Resource, reqparse

import serial
import socket

# Server states
camSecurity = False
changedetected = False

app = Flask(__name__)
api = Api(app)

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


class HelloWorld(Resource):
     def get(self):
        return {'hello': 'world'}

api.add_resource(HelloWorld, '/')

############# Beverage maker functions ##############
@app.route("/beveragemachine/makedrink/<int:num1>/<int:num2>")
def make_drink(num1, num2):
    print(num1, num2)
    return "Ordered drink", 200

############## Liquid functions ################
@app.route("/liquid/get/<string:name>")
def get_liquid(name):
    print("Called this one")
    for liquid in liquids:
        if(name == liquid["name"]):
            return liquid, 200
    return "Liquid not found", 404

@app.route("/liquid/post/<string:name>/<int:volume>")
def post_liquid(name, volume):
    global liquids
    parser = reqparse.RequestParser()
    parser.add_argument("volume")
    args = parser.parse_args()

    for liquid in liquids:
        if(name == liquid["name"]):
            return "Liquid {} already exists",format(name), 400

    liquid = {
        "name": name,
        "volume": volume,
    }

    liquids.append(liquid)
    return liquid, 201

@app.route("/liquid/put/<string:name>/<int:volume>")
def put_liquid(name):
    global liquids
    parser = reqparse.RequestParser()
    parser.add_argument("volume")
    args = parser.parse_args()

    for liquid in liquids:
        if(name == liquid["name"]):
            liquid["volume"] = args["volume"]
            return liquid, 200

    liquid = {
        "name": name,
        "volume": 1000,
    }

    liquids.append(liquid)
    return liquid, 201

@app.route("/liquid/delete/<string:name>")
def delete_liquid(name):
    global liquids
    liquids = [liquid for liquid in liquids if liquid["name"] != name]
    return "Deleted {} liquid.".format("name"), 200

######### Security module functions ##########
@app.route("/securitymodule/detected")
def intruder_detected():
    global changedetected, serCam
    if camSecurity is True:
        if changedetected is True:
            changedetected = False
            
            return {"detected":True}

    return {"detected":False} 

@app.route("/securitymodule/<int:securityonoff>")
def security_switch(securityonoff):
    """Enables or disables camera security"""
    global camSecurity

    if securityonoff == 1:
        camSecurity = True
    elif securityonoff == 0:
        camSecurity = False
    else:
        print("Some strange case")
    
    if camSecurity is True:
        return "Security activated"
    elif camSecurity is False:
        return "Security deactivated"

    return "Error setting security"


if __name__ == "__main__":
    app.run(debug=True)

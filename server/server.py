from flask import Flask, jsonify
from flask_restful import Api, Resource, reqparse

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

class BeverageMachine(Resource):
    def make_cocktail(self, vol1, vol2):
        print(vol1, vol2)

class Liquid(Resource):

    def get(self, name):
        for liquid in liquids:
            if(name == liquid["name"]):
                return liquid, 200
        return "Liquid not found", 404
 
    def post(self, name):
        parser = reqparse.RequestParser()
        parser.add_argument("volume")
        args = parser.parse_args()

        for liquid in liquids:
            if(name == liquid["name"]):
                return "Liquid {} already exists",format(name), 400

        liquid = {
            "name": name,
            "volume": 1000,
        }

        liquids.append(liquid)
        return liquid, 201
 
    def put(self, name):
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
 
    def delete(self, name):
        global liquids
        liquids = [liquid for liquid in liquids if liquid["name"] != name]
        return "Deleted {} liquid.".format("name"), 200


api.add_resource(Liquid, "/liquid/<string:name>")
api.add_resource(BeverageMachine, "/beveragemachine/<int:vol1, int:vol2>")

if __name__ == "__main__":
    app.run(debug=True)

# tasks = [
#     {
#         'id': 1,
#         'title': u'Buy groceries',
#         'description': u'Milk, Cheese, Pizza, Fruit, Tylenol', 
#         'done': False
#     },
#     {
#         'id': 2,
#         'title': u'Learn Python',
#         'description': u'Need to find a good Python tutorial on the web', 
#         'done': False
#     }
# ]

# @app.route('/todo/api/v1.0/tasks', methods=['GET'])
# def get_tasks():
#     return jsonify({'tasks': tasks})

# if __name__ == '__main__':
#     app.run(debug=True)
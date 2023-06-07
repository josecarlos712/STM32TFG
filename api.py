from flask import Flask, jsonify
import json
import random

app = Flask(__name__)

@app.route('/mi-api', methods=['GET'])
def get_data():
    # Aquí generas tu respuesta en formato JSON
    response = {'key': random.randint(1, 10)}

    # Convertir la respuesta en formato JSON a una cadena de texto
    response_str = json.dumps(response)

    # Agregar el sufijo "\r\nOK\r\n> " a la respuesta
    response_str += "\r\nOK\r\n> "

    return response_str

if __name__ == '__main__':
    app.run(host='192.168.18.3', port=5000)

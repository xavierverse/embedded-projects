from flask import Flask, request

app = Flask(__name__)

@app.route('/readfile/<filename>', methods=['GET'])
def read_file(filename):
    file = open(filename, 'r')
    return file.read()

@app.route('/weather', methods=['POST'])
def update_weather():
    data = request.data.decode('utf-8')
    with open('weather_data.txt', 'w') as file:
        file.write(data)
    return "Weather data updated successfully."

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=1234)

from flask import Flask, request, make_response, jsonify, render_template
import mysql.connector

app = Flask(__name__)

mydb = mysql.connector.connect(
    host="localhost",
    user="root",
    password="",
    database="data"
)
mycursor = mydb.cursor()



@app.route('/Group2CTTT', methods=['POST'])
def lights():
    data = request.get_json()
    if data is not None:
        temperature = data['temperature']
        humidity = data['humidity']
        light_intensity = data['light_intensity']
        sql = "INSERT INTO sensordata (devices, location, value1, value2, value3) VALUES (%s, %s, %s, %s, %s)"
        val = ('Wemos D1', 'E3.1', temperature, humidity, light_intensity)
        mycursor.execute(sql, val)
        mydb.commit()
        print(mycursor.rowcount, "record inserted.")
    # Render the HTML template with the data
    return make_response(jsonify({'status': 'OK'}), 200)


@app.route('/', methods=['GET'])
def index():
    sql = "SELECT * FROM sensordata"
    mycursor.execute(sql)
    rows = mycursor.fetchall()
    row = rows[-1]
    time = row[6]
    return render_template('index.html', time=time)


@app.route('/index.html', methods=['GET'])
def index1():
    return index()


@app.route('/current_value.html', methods=['GET'])
def currentvalue():
    sql = "SELECT * FROM sensordata"
    mycursor.execute(sql)
    rows = mycursor.fetchall()
    row = rows[-1]
    temperature = row[3]
    humidity = row[4]
    light_intensity = row[5]
    return render_template('current_value.html', temperature=temperature, humidity=humidity,
                           light_intensity=light_intensity)


if __name__ == '__main__':
    app.run('172.31.0.51', 8000)

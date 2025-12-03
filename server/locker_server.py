from flask import Flask, render_template, request, jsonify
import psycopg2
from psycopg2.extras import RealDictCursor
import os
import paho.mqtt.client
import certifi
import json

app = Flask(__name__)

def on_connect(mitt, dados_usuario, flags, codigo):
    pass

def on_message(cliente, dados_usuario, mensagem):
    pass

mqtt = paho.mqtt.client.Client()
mqtt.tls_set(certifi.where())
mqtt.username_pw_set(username="aula", password="zowmad-tavQez")
mqtt.on_connect = on_connect
mqtt.on_message = on_message
mqtt.connect("mqtt.janks.dev.br", port=8883, keepalive=10)

# Database configuration
DB_CONFIG = {
    'host': os.getenv('DB_HOST', 'postgresql.janks.dev.br'),
    'database': os.getenv('DB_NAME', 'projeto'),
    'user': os.getenv('DB_USER', 'iot'),
    'password': os.getenv('DB_PASSWORD', 'pepcon-garton'),
    'port': os.getenv('DB_PORT', '5432')
}

def get_db_connection():
    """Create a database connection"""
    conn = psycopg2.connect(**DB_CONFIG)
    return conn

def init_db():
    """Initialize the database with a sample table"""
    conn = get_db_connection()
    cursor = conn.cursor()

    # Check if table is empty
    cursor.execute('SELECT COUNT(*) FROM lockers')
    count = cursor.fetchone()[0]

    if count == 0:
        # Insert sample data
        sample_data = [
            (True,),
            (False,),
            (True,),
            (False,),
        ]
        cursor.executemany(
            'INSERT INTO lockers (active) VALUES (%s)',
            sample_data
        )

    conn.commit()
    cursor.close()
    conn.close()
    print("Database initialized successfully!")

@app.route('/')
def index():
    """Main page showing all lockers"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor(cursor_factory=RealDictCursor)
        cursor.execute('SELECT * FROM lockers ORDER BY id')
        lockers = cursor.fetchall()
        cursor.close()
        conn.close()

        return render_template('index.html', lockers=lockers)
    except Exception as e:
        return f"Database error: {str(e)}", 500

@app.route('/toggle/<int:locker_id>')
def toggle_locker(locker_id):
    """Toggle locker active status"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor(cursor_factory=RealDictCursor)

        # Get current status
        cursor.execute('SELECT active FROM lockers WHERE id = %s', (locker_id,))
        locker = cursor.fetchone()

        if locker is None:
            cursor.close()
            conn.close()
            return "Locker not found", 404

        # Toggle status
        new_status = not locker['active']
        cursor.execute(
            'UPDATE lockers SET active = %s WHERE id = %s',
            (new_status, locker_id)
        )
        conn.commit()
        cursor.close()
        conn.close()
        
        data = {
        "locker_id": locker_id,
        "active": new_status
        }      
        
        mqtt.publish("locker-override", "oi")

        mqtt.publish("locker-override", json.dumps(data))
        return render_template('index.html', lockers=get_all_lockers())
    except Exception as e:
        return f"Error: {str(e)}", 500

def get_all_lockers():
    """Helper function to get all lockers"""
    conn = get_db_connection()
    cursor = conn.cursor(cursor_factory=RealDictCursor)
    cursor.execute('SELECT * FROM lockers ORDER BY id')
    lockers = cursor.fetchall()
    cursor.close()
    conn.close()
    return lockers

@app.route('/add')
def add_locker():
    """Add a new locker"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute('INSERT INTO lockers (active) VALUES (%s)', (False,))
        conn.commit()
        cursor.close()
        conn.close()

        return render_template('index.html', lockers=get_all_lockers())
    except Exception as e:
        return f"Error: {str(e)}", 500

@app.route('/delete/<int:locker_id>')
def delete_locker(locker_id):
    """Delete a locker"""
    try:
        conn = get_db_connection()
        cursor = conn.cursor()
        cursor.execute('DELETE FROM lockers WHERE id = %s', (locker_id,))
        conn.commit()
        cursor.close()
        conn.close()

        return render_template('index.html', lockers=get_all_lockers())
    except Exception as e:
        return f"Error: {str(e)}", 500


if __name__ == '__main__':
    try:
        init_db()
    except Exception as e:
        print(f"Warning: Could not initialize database: {e}")
        print("Make sure PostgreSQL is running and credentials are correct.")

    app.run(debug=True, host='0.0.0.0', port=5000)

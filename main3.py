import re
import os
import cv2
from flask import Flask, render_template, Response
from pycoral.utils.dataset import read_label_file
from pycoral.utils.edgetpu import make_interpreter
from pycoral.adapters import common
from pycoral.adapters import classify

app = Flask(__name__)

# the TFLite converted to be used with edgetpu
modelPath = './model_edgetpu.tflite'

# The path to labels.txt that was downloaded with your model
labelPath = './labels.txt'

# This function takes in a TFLite Interpreter and Image, and returns classifications
def classifyImage(interpreter, image):
    size = common.input_size(interpreter)
    common.set_input(interpreter, cv2.resize(image, size, fx=0, fy=0,
                                             interpolation=cv2.INTER_CUBIC))
    interpreter.invoke()
    return classify.get_classes(interpreter)

def gen_frames():  
    interpreter = make_interpreter(modelPath)
    interpreter.allocate_tensors()
    labels = read_label_file(labelPath)

    cap = cv2.VideoCapture(1)
    while True:
        ret, frame = cap.read()
        if not ret:
            break

        # Flip image so it matches the training input
        frame = cv2.flip(frame, 1)

        # Classify and display image
        results = classifyImage(interpreter, frame)


	if results and results[0].score > 0.9:
        	height, width = frame.shape[:2]
        	top_left = (int(width / 4), int(height / 4))
		bottom_right = (int(width * 3 / 4), int(height * 3 / 4))
            	cv2.rectangle(frame, top_left, bottom_right, (0, 255, 0), 3)
            	label = f'{labels[results[0].id]}: {results[0].score:.2%}'
            	cv2.putText(frame, label, (int(width / 4), int(height / 4) - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2)

        print(f'Label: {labels[results[0].id]}, Score: {results[0].score}')        
        ret, buffer = cv2.imencode('.jpg', frame)
        frame = buffer.tobytes()
        yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port='5000', debug=True)


# This is a sample Python script.

# Press ⌃R to execute it or replace it with your code.
# Press Double ⇧ to search everywhere for classes, files, tool windows, actions, and settings.


from PIL import Image
import numpy as np
import cv2
from flask import Flask, render_template, Response
import tflite_runtime.interpreter as tflite

app = Flask(__name__)

labels = [line.rstrip() for line in open('labels.txt')]

# Load TFLite model and allocate tensors.
interpreter = tflite.Interpreter(model_path="model_edgetpu.tflite", experimental_delegates=[tflite.load_delegate('libedgetpu.so.1')])
interpreter.allocate_tensors()

def classify_image(interpreter, image, top_k=1):
    # Details of the model input and output
    input_details = interpreter.get_input_details()[0]
    output_details = interpreter.get_output_details()[0]
    
    # Make sure the input data type is correct
    image = image.astype(input_details["dtype"])

    # Set the tensor to point to the input data to be inferred
    interpreter.set_tensor(input_details["index"], image)

    # Run the inference
    interpreter.invoke()
    
    # Get the output from the inference
    output = interpreter.get_tensor(output_details["index"])

    # Get the highest scoring class
    top_k_indices = np.argsort(output)[::-1][:top_k]
    return [(i, output[i]) for i in top_k_indices]

def gen_frames():  
    camera = cv2.VideoCapture(0)
    while True:
        success, frame = camera.read()  
        if not success:
            break
        else:
            # resize and reshape the frame to fit model input
            img = Image.fromarray(frame).resize((224, 224))
            img = np.array(img).reshape((1, 224, 224, 3))

            # classify the image
            results = classify_image(interpreter, img)
            label, score = labels[results[0][0]], results[0][1] * 100
            
            # if score more than 90%, display the match
            if score > 90:
                cv2.putText(frame, f"{label}: {score:.2f}%", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)
            
            # convert image color to make it viewable
            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
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


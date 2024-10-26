from mediapipe.framework.formats import landmark_pb2
from mediapipe import solutions
from mediapipe.tasks import python
from mediapipe.tasks.python import vision
import cv2
import json
import mediapipe as mp
import numpy as np
import os

# Add this near the top of your script, after your imports
cv2.namedWindow('Face Mesh and Hands', cv2.WINDOW_NORMAL)

from dataclasses import dataclass
from typing import Tuple, Dict

@dataclass
class DrawingSpec:
    color: Tuple[int, int, int]
    thickness: int

    FACEMESH_LEFT_IRIS = mp.solutions.face_mesh.FACEMESH_LEFT_IRIS
    FACEMESH_RIGHT_IRIS = mp.solutions.face_mesh.FACEMESH_RIGHT_IRIS

def iris_connection_style():
    face_mesh_iris_connections_style = {}
    left_spec = DrawingSpec(color=(0, 255, 0), thickness=2)
    right_spec = DrawingSpec(color=(255, 0, 0), thickness=2)
    for connection in FACEMESH_LEFT_IRIS:
        face_mesh_iris_connections_style[connection] = left_spec
    for connection in FACEMESH_RIGHT_IRIS:
        face_mesh_iris_connections_style[connection] = right_spec
    return face_mesh_iris_connections_style

def draw_landmarks_on_image(image, face_detection_result, hand_detection_result):
    annotated_image = np.zeros_like(image)
    if face_detection_result.face_landmarks:
        for face_landmarks in face_detection_result.face_landmarks:
            face_landmarks_proto = landmark_pb2.NormalizedLandmarkList()
            face_landmarks_proto.landmark.extend([
                landmark_pb2.NormalizedLandmark(x=landmark.x, y=landmark.y, z=landmark.z) for landmark in face_landmarks
            ])

            solutions.drawing_utils.draw_landmarks(
                image=annotated_image,
                landmark_list=face_landmarks_proto,
                connections=mp.solutions.face_mesh.FACEMESH_TESSELATION,
                landmark_drawing_spec=None,
                connection_drawing_spec=mp.solutions.drawing_styles.get_default_face_mesh_tesselation_style())
            solutions.drawing_utils.draw_landmarks(
                image=annotated_image,
                landmark_list=face_landmarks_proto,
                connections=mp.solutions.face_mesh.FACEMESH_CONTOURS,
                landmark_drawing_spec=None,
                connection_drawing_spec=mp.solutions.drawing_styles.get_default_face_mesh_contours_style())
            solutions.drawing_utils.draw_landmarks(
                image=annotated_image,
                landmark_list=face_landmarks_proto,
                connections=mp.solutions.face_mesh.FACEMESH_IRISES,
                landmark_drawing_spec=None,
                connection_drawing_spec=iris_connection_style())

    # draw hand landmarks
    if hand_detection_result.hand_landmarks:
        for hand_landmarks in hand_detection_result.hand_landmarks:
            hand_landmarks_proto = landmark_pb2.NormalizedLandmarkList()
            hand_landmarks_proto.landmark.extend([
                landmark_pb2.NormalizedLandmark(x=landmark.x, y=landmark.y, z=landmark.z) for landmark in hand_landmarks
            ])
            solutions.drawing_utils.draw_landmarks(
                annotated_image,
                hand_landmarks_proto,
                mp.solutions.hands.HAND_CONNECTIONS,
                mp.solutions.drawing_styles.get_default_hand_landmarks_style(),
                mp.solutions.drawing_styles.get_default_hand_connections_style())

    return annotated_image

# create facelandmarker and handlandmarker with gpu support
face_model_path = os.path.join('models', 'face_landmarker.task')
hand_model_path = os.path.join('models', 'hand_landmarker.task')

base_options = python.BaseOptions(model_asset_path=face_model_path, delegate=python.BaseOptions.Delegate.GPU)
face_options = vision.FaceLandmarkerOptions(base_options=base_options,
                                            output_face_blendshapes=True,
                                            output_facial_transformation_matrixes=True,
                                            num_faces=1)
face_detector = vision.FaceLandmarker.create_from_options(face_options)

base_options = python.BaseOptions(model_asset_path=hand_model_path, delegate=python.BaseOptions.Delegate.GPU)
hand_options = vision.HandLandmarkerOptions(base_options=base_options,
                                            num_hands=2)
hand_detector = vision.HandLandmarker.create_from_options(hand_options)

# initialize webcam with lower resolution
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 800)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 600)
cap.set(cv2.CAP_PROP_FPS, 60)

# target display size
display_width, display_height = 800, 600

from collections import deque

jaw_open_values = deque(maxlen=40)

cv2.resizeWindow('Face Mesh and Hands', 1024, 768)  # or whatever size you want
while True:
    ret, frame = cap.read()
    frame = cv2.flip(frame, 1)
    if not ret:
        break

    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=rgb_frame)

    face_result = face_detector.detect(mp_image)
    hand_result = hand_detector.detect(mp_image)

    frame_data = {"face_landmarks": [], "hand_landmarks": []}

    if face_result.face_landmarks:
        for blendshapes in face_result.face_blendshapes:
            jaw_open = next((b.score for b in blendshapes if b.category_name == 'jawOpen'), None)
            if jaw_open is not None:
                jaw_open_values.append(jaw_open)

        if len(jaw_open_values) == 40 and all(v > 0.02 for v in jaw_open_values):
            print("SOY FACE!!!!!!!!")
        else:
            print("._.")

        frame_data["face_landmarks"] = [[{"x": p.x, "y": p.y, "z": p.z} for p in face] for face in face_result.face_landmarks]

    if hand_result.hand_landmarks:
        frame_data["hand_landmarks"] = [[{"x": p.x, "y": p.y, "z": p.z} for p in hand] for hand in hand_result.hand_landmarks]

    with open('landmark_data.json', 'w') as f:
        json.dump(frame_data, f)

    annotated_image = draw_landmarks_on_image(rgb_frame, face_result, hand_result)
    annotated_image = cv2.cvtColor(annotated_image, cv2.COLOR_RGB2BGR)
    display_image = cv2.resize(annotated_image, (display_width, display_height), interpolation=cv2.INTER_LINEAR)

    cv2.imshow('Face Mesh and Hands', display_image)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()


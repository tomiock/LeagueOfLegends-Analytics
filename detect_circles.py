import cv2
import numpy as np


def detect_circles(image, radius, param1=200, param2=20, tolerance=3):
    min_r = radius - tolerance
    max_r = radius + tolerance

    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    detected_circles = cv2.HoughCircles(gray,
                                        cv2.HOUGH_GRADIENT, 1, 20,
                                        param1=param1,
                                        param2=param2,
                                        minRadius=min_r,
                                        maxRadius=max_r)

    return detected_circles


def draw_circles(output_image, detected_circles):
    if detected_circles is not None:

        detected_circles = np.uint16(np.around(detected_circles))

        for pt in detected_circles[0, :]:
            a, b, r = pt[0], pt[1], pt[2]

            cv2.circle(output_image, (a, b), r, (0, 255, 0), 2)
            cv2.circle(output_image, (a, b), 1, (0, 0, 255), 3)
    return output_image


if __name__ == "__main__":
    RADIUS = 30

    img = cv2.imread('test_images/IMG_11.png')
    output_image = cv2.imread('test_images/IMG_11.png')

    circles = detect_circles(
        img, RADIUS, param1=300, param2=20, tolerance=3)

    img = draw_circles(output_image, circles)

    cv2.imshow("Detected Circles", img)
    cv2.waitKey(0)

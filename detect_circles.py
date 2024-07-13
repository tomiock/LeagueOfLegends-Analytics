import cv2
import numpy as np


def detect_circles(image, radius, output_image=None, param1=200, param2=20):
    if output_image is None:
        output_image = np.copy(image)

    min_r = radius - 3
    max_r = radius + 3

    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    detected_circles = cv2.HoughCircles(gray,
                                        cv2.HOUGH_GRADIENT, 1, 20,
                                        param1=param1,
                                        param2=param2,
                                        minRadius=min_r,
                                        maxRadius=max_r)

    if detected_circles is not None:

        detected_circles = np.uint16(np.around(detected_circles))

        for pt in detected_circles[0, :]:
            a, b, r = pt[0], pt[1], pt[2]

            cv2.circle(output_image, (a, b), r, (0, 255, 0), 2)

            cv2.circle(output_image, (a, b), 1, (0, 0, 255), 3)
    return output_image


if __name__ == "__main__":
    RADIUS = 19

    img = cv2.imread('test_images/IMG_11_clean.png')
    output_image = cv2.imread('test_images/IMG_11_clean.png')

    img = detect_circles(img, RADIUS, output_image=output_image, param1=300, param2=22)

    cv2.imshow("Detected Circles", img)
    cv2.waitKey(0)

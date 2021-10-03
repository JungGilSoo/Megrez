//
// Created by benjamin on 26.03.20.
//
#ifndef VISUALS_FACEDETECTOR_H
#define VISUALS_FACEDETECTOR_H

class FaceDetector {
public:
    explicit FaceDetector();

/// Detect face in an image frame
/// \param frame Imgage to detect faces in
/// \return Vector of detected faces
    std::vector<cv::Rect> detect_face_rectangles(const cv::Mat &frame);    

};

#endif //VISUALS FACEDETECTOR_H
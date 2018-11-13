#include <string>

enum DetectionMethod {
    RECTANGULAR, 
    SIMPLE_RECTANGULAR, 
    CIRCULAR, 
    SIMPLE_CIRCULAR, 
    CIRCULAR_SECTOR,
    SIMPLE_CIRCULAR_SECTOR`
};

enum AreaType {
    RECTANGULAR, CIRCULAR
};

struct DetectionArea {
    AreaType type;
    double center_x;
    double center_y;
    double radius; // circular
    double width; //rectangular
    double length;//rectangular
    double x;
    double y;
    double x1; //rectangular
    double x2; //rectangular
} detection_area;

class ObjectDetection {
    public:
        ObjectDetection() {

        }

        void setDetectionMethod(DetectionMethod method) {
            
        }

        void addDetectionArea(DetectionArea area) {

        }

        void processDetection() {
            
        }

        
    private:
        DetectionMethod detection_method;
        DetectionArea *detectionAreas;
        int _detectionAreaIndex;
        
        
}
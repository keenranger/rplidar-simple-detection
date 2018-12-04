#include <string>

enum DetectionMethod
{
    RECTANGULAR,
    SIMPLE_RECTANGULAR,
    CIRCULAR,
    SIMPLE_CIRCULAR,
    CIRCULAR_SECTOR,
    SIMPLE_CIRCULAR_SECTOR`
};

enum AreaType
{
    RECTANGULAR,
    CIRCULAR
};

struct DetectionArea
{
    AreaType type;
    double center_x;
    double center_y;
    double radius; // circular
    double width;  //rectangular
    double length; //rectangular
    double x;
    double y;
    double x1; //rectangular
    double x2; //rectangular
} detection_area;

class ObjectDetection
{
  public:
    ObjectDetection()
    {
    }

    void setDetectionMethod(DetectionMethod method){}

    void addDetectionArea(DetectionArea area) {}

    void processDetection(double angle, int quality)
    {
        if (angle > 70 && angle <= 90)
        {
            if (quality != 0 && distance != 0)
            {
                if (minDisLeft > distance)
                {
                    minDisLeft = distance;
                }
            }
            else if (quality != 0 && distance == 0)
                state = 0;
        }
        else if (angle > 90 && angle <= 110)
        {
            if (quality != 0 && distance != 0)
            {
                if (minDisRight > distance)
                {
                    minDisRight = distance;
                }
            }
            else if (quality != 0 && distance == 0)
                state = 0;
        }
    }

    int processState()
    {
        if (minDisLeft < 2000 && state != 0)
        {
            state = 3;
        }
        if (minDisRight < 2000 && state != 0)
        {
            state = 2;
            if (minDisLeft < 2000)
            {
                if (minDisLeft < minDisRight)
                {
                    state = 3;
                }
            }
        }
        if (minDisLeft < 800 || minDisRight < 800)
        {
            state = 4;
        }
    }

    void setDistance(double distance)
    {
        distance = distance
    }

  private:
    DetectionMethod detection_method;
    DetectionArea *detectionAreas;
    int _detectionAreaIndex;
    int minDisLeft;
    int minDisRight;
    int state;
    double distance;
}
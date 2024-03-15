#include "operator.h"
float k = 0;
bool rising = true;
void processOperator(struct Operator *operator)
{
    const char *TAG = "processOperator";
    // ESP_LOGI(TAG, " ");
    if (rising)
    {
        if (k < 1)
            k += 0.01;
        else
            rising = false;
    }
    else
    {
        if (k > -1)
            k -= 0.01;
        else
            rising = true;
    }
    operator->out = k;
}
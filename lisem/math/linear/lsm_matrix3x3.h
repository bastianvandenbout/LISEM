#ifndef MATRIX3X3_H
#define MATRIX3X3_H


class LSMMatrix3x3
{

public:
    float data[3][3];

    inline LSMMatrix3x3()
    {
        SetIdentity();

    }


    inline void SetIdentity()
    {
        data[0][0] = 1.0f;
        data[0][1] = 0.0f;
        data[0][2] = 0.0f;
        data[1][0] = 0.0f;
        data[1][1] = 1.0f;
        data[1][2] = 0.0f;
        data[2][0] = 0.0f;
        data[2][1] = 0.0f;
        data[2][2] = 1.0f;
    }


    inline float * GetMatrixData()
    {
        float *ret = new float[9];

        for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                ret[3*i +j] = data[i][j];
            }
        }
        return ret;
    }

    inline void Transpose()
    {
        for (int i = 0; i < 3; i++) {
            for (int j = i+1; j < 3; j++) {
                double temp = data[i][j];
                data[i][j] = data[j][i];
                data[j][i] = temp;
            }
        }
    }

    inline void MultiplyBy(const LSMMatrix3x3 &other)
    {
        float temp[3][3];

        for (int row = 0; row < 3; ++row) {
                 for (int col = 0; col < 3; ++col) {
                     float sum(0.0f);
                     for (int j = 0; j < 3; ++j)
                     {
                         sum += data[j][row] * other.data[col][j];
                     }
                    temp[col][row] = sum;
                }
            }
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                data[i][j] = temp[i][j];
            }
        }


    }

};

#endif // MATRIX3X3_H



#include "defines.h"

#include <vector>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn_superres.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "lsmio.h"
#include "geo/raster/map.h"
#include "error.h"
#include "raster/rasternetwork.h"


inline std::vector<cTMap *> ApplyMLBModel(std::vector<cTMap *> Maps, QString modelpath, int patchsize_x, int patchsize_y, std::vector<int> inputorder, bool fill = false)
{

    if(patchsize_x <= 0 || patchsize_y <= 0)
    {
        LISEMS_ERROR("Can not apply Ml model with patch size of 0");
        throw 1;
    }
    if(inputorder.size() == 0)
    {
        inputorder.push_back(0);
        inputorder.push_back(1);
        inputorder.push_back(2);
        inputorder.push_back(3);
    }
    if(inputorder.size() != 4)
    {
        LISEMS_ERROR("Invalid input size, must be array of length 4 with items 0,1,2,3 in an order");
        throw 1;
    }

    std::cout << "check input order" << std::endl;

    bool i1f = false; bool i2f = false; bool i3f = false; bool i0f = false;
    for(int i  = 0; i < inputorder.size(); i++)
    {
        if(inputorder.at(i) == 0)
        {
            i0f = true;
        }
        if(inputorder.at(i) == 1)
        {
            i1f = true;
        }
        if(inputorder.at(i) == 2)
        {
            i2f = true;
        }
        if(inputorder.at(i) == 3)
        {
            i3f = true;
        }
    }
    if(!(i0f && i1f && i2f && i3f))
    {
        LISEMS_ERROR("Invalid input size, must be array of length 4 with items 0,1,2,3 in an order");
        throw 1;
    }
    //Create the module's object

    std::vector<cTMap *> valueTotal;
    std::vector<cTMap *> WeightTotal;

    //now apply the ml model to patches of the patch size


    cv::dnn::Net net;

    std::cout << "read net " << std::endl;

    //Read the desired model
    std::string path = (AS_DIR+modelpath).toStdString();
    net = cv::dnn::readNet(path);


    std::vector<std::string> names =  net.getLayerNames();
    for(int i = 0; i < names.size(); i++)
    {
        std::cout << "info " << names.at(i) << std::endl;
    }


    std::cout << "net loaded " << std::endl;

    int dx[LDD_DIR_LENGTH] = LDD_X_LIST;
    int dy[LDD_DIR_LENGTH] = LDD_Y_LIST;
    float dist[LDD_DIR_LENGTH] = LDD_DIST_LIST;



    int patches_x= 2.0 * (Maps.at(0)->nrCols()/(patchsize_x)) + 2;
    int patches_y= 2.0 * (Maps.at(0)->nrRows()/(patchsize_y)) + 2;

    bool first = true;
    int rescale_x = 1;
    int rescale_y = 1;
    for(int i = 0; i < patches_x; i++)
    {
        for(int j = 0; j < patches_y; j++)
        {


            std::cout << "patch "<< i << " " << j << std::endl;

            int r_min = -patchsize_y/2 + j * patchsize_y*0.5;
            int r_max = -patchsize_y/2 + (j)* patchsize_y*0.5 + patchsize_y;
            int c_min = -patchsize_x/2 + i * patchsize_x*0.5;
            int c_max = -patchsize_x/2 + (i) * patchsize_x*0.5 + patchsize_x;

            //set input blob

            std::cout << "set size" << std::endl;

           std::vector<int> outShape(4);
           outShape[inputorder[0]] = 1;  // batch size
           outShape[inputorder[1]] = Maps.size();  // number of channels
           outShape[inputorder[2]] = patchsize_x;
           outShape[inputorder[3]] = patchsize_y;


           std::cout << "dims "<< r_min << " " << r_max << "  " << c_min << " " << c_max <<  std::endl;

           bool skip = false;
           cv::Mat blob = cv::Mat(4, outShape.data(), CV_32F);
           for(int b = 0; b < Maps.size(); b++)
           {
               double av_total = 0.0;
               double av_count = 0.0;
               for(int r = 0; r < patchsize_y; r++)
               {
                   for(int c = 0; c < patchsize_x; c++)
                   {

                           std::vector<int> index(4);
                           index[inputorder[0]] = 0;  // batch size
                           index[inputorder[1]] = b;  // number of channels
                           index[inputorder[2]] = c;
                           index[inputorder[3]] = r;

                           if((r_min + r > -1 && r_min + r < Maps.at(b)->nrRows())
                                   &&
                              (c_min + c > -1 && c_min + c < Maps.at(b)->nrCols())
                               )
                           {
                               if(!pcr::isMV(Maps.at(b)->data[r_min + r][c_min + c]))
                               {
                                    blob.at<float>(index.data()) = Maps.at(b)->data[r_min + r][c_min + c];
                                    av_total +=Maps.at(b)->data[r_min + r][c_min + c];
                                    av_count += 1.0;
                               }else
                               {
                                   pcr::setMV(blob.at<float>(index.data()));
                               }
                           }else
                           {
                               pcr::setMV(blob.at<float>(index.data()));
                           }

                   }
               }

               if(av_count > 0.5)
               {
                   av_total = av_total/av_count;
               }else
               {
                   std::cout<< "only mv,skip "<<std::endl;
                skip = true;
                break;
               }


               if(fill)
               {
                   bool change = true;
                   while(change)
                   {
                       change = false;

                       for(int r = 0; r < patchsize_y; r++)
                       {
                           for(int c = 0; c < patchsize_x; c++)
                           {

                                   std::vector<int> index(4);
                                   index[inputorder[0]] = 0;  // batch size
                                   index[inputorder[1]] = b;  // number of channels
                                   index[inputorder[2]] = c;
                                   index[inputorder[3]] = r;

                                   if(pcr::isMV(blob.at<float>(index.data())))
                                   {

                                       double pix_av = 0.0;
                                       double pix_tot = 0.0;

                                       for(int ldd = 0; ldd  < LDD_DIR_LENGTH; ldd++)
                                       {
                                           if(!(ldd == LDD_PIT))
                                           {
                                               std::vector<int> index2(4);
                                               index2[inputorder[0]] = 0;  // batch size
                                               index2[inputorder[1]] = b;  // number of channels
                                               index2[inputorder[2]] = c+ dx[i];
                                               index2[inputorder[3]] = r+ dy[i];

                                               if(c+ dx[i] > -1 && c+ dx[i] < patchsize_x && r+ dy[i] > -1 && r+ dy[i] < patchsize_y)
                                               {
                                                   if(!pcr::isMV(blob.at<float>(index2.data())))
                                                   {
                                                       pix_av +=  blob.at<float>(index2.data());
                                                       pix_tot += 1.0;
                                                   }
                                               }
                                           }
                                       }

                                       if(pix_tot > 0.5)
                                       {
                                           change = true;
                                           blob.at<float>(index.data()) = pix_av/pix_tot;
                                       }
                                   }
                           }
                       }
                   }


               }else
               {
                   for(int r = 0; r < patchsize_y; r++)
                   {
                       for(int c = 0; c < patchsize_x; c++)
                       {

                               std::vector<int> index(4);
                               index[inputorder[0]] = 0;  // batch size
                               index[inputorder[1]] = b;  // number of channels
                               index[inputorder[2]] = c;
                               index[inputorder[3]] = r;

                               if(pcr::isMV(blob.at<float>(index.data())))
                               {
                                   blob.at<float>(index.data()) = av_total;

                                }
                       }
                   }
               }



           }

           if(skip)
           {
               continue;
           }

           net.setInput(blob);
           std::cout << "predict patch " << i << "  " << j << std::endl;
           cv::Mat blob_output = net.forward();

           std::cout << "predict patch done " << i << "  " << j << std::endl;


           std::cout << "blob out" << blob_output.size[0] << " " << blob_output.size[1] << " " <<  blob_output.size[2] <<  " " << blob_output.size[3] << std::endl;

           if(first)
           {

               std::cout << "first " <<blob_output.size[inputorder[0]] << " " << blob_output.size[inputorder[1]] << " " << blob_output.size[inputorder[2]] << " " << blob_output.size[inputorder[3]] << " " <<  std::endl;

               int new_bands = blob_output.size[inputorder[1]];

               std::cout << "new bands "<< new_bands << std::endl;
               std::cout << " new image " << blob_output.size[inputorder[2]] << " " << blob_output.size[inputorder[3]] << std::endl;
               for(int b = 0; b < new_bands; b++)
               {
                   int new_size_x = (blob_output.size[inputorder[2]] * Maps.at(b)->nrCols())/patchsize_x;
                   int new_size_y = (blob_output.size[inputorder[3]] * Maps.at(b)->nrRows())/patchsize_y;

                   if(first && b == 0)
                   {
                        rescale_x = blob_output.size[inputorder[2]]/patchsize_x;
                        rescale_y = blob_output.size[inputorder[3]]/patchsize_y;

                        std::cout << "first " << rescale_x << " " << rescale_y <<  std::endl;
                   }
                   MaskedRaster<float> raster_data(new_size_y, new_size_x, Maps.at(0)->data.north(), Maps.at(0)->data.west(), Maps.at(0)->data.cell_size(),Maps.at(0)->data.cell_sizeY());
                   cTMap *patchi = new cTMap(std::move(raster_data),Maps.at(0)->projection(),"");
                   valueTotal.push_back(patchi);

                   MaskedRaster<float> raster_data2(new_size_y, new_size_x, Maps.at(0)->data.north(), Maps.at(0)->data.west(), Maps.at(0)->data.cell_size(), Maps.at(0)->data.cell_sizeY());
                   cTMap *patchwi = new cTMap(std::move(raster_data2),Maps.at(0)->projection(),"");
                   WeightTotal.push_back(patchwi);
               }
           }

           if(first)
           {
               first = false;
           }
           std::cout << "add data " << std::endl;
           for(int b = 0; b < Maps.size(); b++)
           {
               for(int r = 0; r <(rescale_y*patchsize_y); r++)
               {
                   for(int c = 0; c < (rescale_x*patchsize_x); c++)
                   {

                       int r_min = -(rescale_y*patchsize_y)/2 + j * (rescale_y*patchsize_y)*0.5;
                       int r_max = -(rescale_y*patchsize_y)/2 + (j)* (rescale_y*patchsize_y)*0.5 + (rescale_y*patchsize_y);
                       int c_min = -(rescale_x*patchsize_x)/2 + i * (rescale_x*patchsize_x)*0.5;
                       int c_max = -(rescale_x*patchsize_x)/2 + (i) * (rescale_x*patchsize_x)*0.5 + (rescale_y*patchsize_x);


                       float weightx = std::max(0.0,1.0 - 1.0 *(std::fabs(0.5 * (rescale_y*patchsize_y) - r)/(0.5 * (rescale_y*patchsize_y))));
                       float weighty = std::max(0.0,1.0 - 1.0 *(std::fabs(0.5 * (rescale_x*patchsize_x) - c)/(0.5 * (rescale_x*patchsize_x))));
                       float weight = weightx * weighty;

                       std::vector<int> index(4);
                       index[inputorder[0]] = 0;  // batch size
                       index[inputorder[1]] = b;  // number of channels
                       index[inputorder[2]] = c;
                       index[inputorder[3]] = r;

                       if((r_min + r > -1 && r_min + r < valueTotal.at(b)->nrRows())
                               &&
                          (c_min + c > -1 && c_min + c < valueTotal.at(b)->nrCols())
                           )
                       {
                           if(!pcr::isMV(blob_output.at<float>(index.data())))
                           {

                                valueTotal.at(b)->data[r_min + r][c_min + c] += weight * blob_output.at<float>(index.data());
                                WeightTotal.at(b)->data[r_min + r][c_min + c] += weight;
                           }
                       }
                   }
               }
           }
        }
    }

    //finally, estimate the blended values

   for(int i = 0; i < valueTotal.size(); i++)
   {
       float ratio_x = ((float)(valueTotal.at(i)->nrCols()))/((float)(Maps.at(i)->nrCols()));
       float ratio_y = ((float)(valueTotal.at(i)->nrRows()))/((float)(Maps.at(i)->nrRows()));

       valueTotal.at(i)->setcellSizeX(valueTotal.at(i)->cellSizeX() / ratio_x);
       valueTotal.at(i)->setcellSizeY(valueTotal.at(i)->cellSizeY() / ratio_y);
   }

   for(int i = 0; i < valueTotal.size(); i++)
   {
       for(int r = 0; r <valueTotal.at(i)->nrRows(); r++)
       {
           for(int c = 0; c < valueTotal.at(i)->nrCols(); c++)
           {
               if(WeightTotal.at(i)->data[r][c] > 1e-10)
               {
                   valueTotal.at(i)->data[r][c] =  valueTotal.at(i)->data[r][c]/WeightTotal.at(i)->data[r][c];
               }

           }
       }

   }

   return valueTotal;


}


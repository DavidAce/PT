//
// Created by david on 2016-08-05.
//

#include <nmspc_math_algorithms.h>


namespace math {

    ArrayXXd mean_depthwise(std::vector<ArrayXXd> &array3D) {
        auto rows = (int)array3D[0].rows();
        auto cols = (int)array3D[0].cols();
        auto dept = (int)array3D.size();
        ArrayXXd result(rows,cols);
        ArrayXd  depth_array(dept);
        for(auto j = 0 ; j < cols; j++ ){
            for(auto i = 0; i < rows; i++){
                for(auto k = 0; k < dept; k++){
                    depth_array(k) = array3D[k](i,j);
                }
                if(depth_array.hasNaN()){
                    result(i,j) = std::numeric_limits<double>::quiet_NaN();
                }
                result(i,j) = depth_array.mean();
            }
        }
        return result;
    }

    ArrayXXd err_depthwise(std::vector<ArrayXXd> &array3D, ArrayXXd &avg) {
        auto rows = array3D[0].rows();
        auto cols = array3D[0].cols();
        auto dept = array3D.size();
        ArrayXXd result(rows,cols);
        ArrayXd  depth_array(dept);
        for(auto j = 0 ; j < cols; j++ ){
            for(auto i = 0; i < rows; i++){
                for(unsigned long int k = 0; k < dept; k++){
                    depth_array(k) = array3D[k](i,j);
                }
                if (depth_array.hasNaN()){
                    result(i,j) = std::numeric_limits<double>::quiet_NaN();
                }else{
                    result(i,j) = sqrt((depth_array - avg(i,j)).cwiseAbs2().sum()/(dept-1));
                }
//                result(i,j) = depth_array.mean();
            }
        }
        return result;
    }

    int mod2(const int &a, const int &b) {
        if (b < 0) {
            return mod(-a, -b);
        }
        int ret = a % b;
        if (ret < 0) {
            ret += b;
        }
        return ret;
    }



}

#include "kalman_filter.h"

#include <iostream>
#include <math.h>

using Eigen::MatrixXd;
using Eigen::VectorXd;

using namespace std;

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
  TODO:
    * predict the state
  */

  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();
  P_ = F_ * P_ * Ft + Q_;

}

void KalmanFilter::Update(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Kalman Filter equations
  */

  VectorXd z_pred = H_ * x_;
  VectorXd y = z - z_pred;
  MatrixXd Ht = H_.transpose();
  MatrixXd S = H_ * P_ * Ht + R_;
  MatrixXd Si = S.inverse();
  MatrixXd PHt = P_ * Ht;
  MatrixXd K = PHt * Si;

  // new estimate
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_) * P_; 

}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
  TODO:
    * update the state by using Extended Kalman Filter equations
  */

  float px = x_(0);
  float py = x_(1);
  float vx = x_(2);
  float vy = x_(3);
  
  float px_py_2 = pow(px, 2.0) + pow(py, 2.0);

   if (px_py_2 == 0) {
     fprintf(stderr, "Division by zero\n");
     return;
   }

   if (px == 0) {
     fprintf(stderr, "Division by zero\n");
     return;
   }

  float sqrt_pxpy2 = sqrt(px_py_2);
  float arctan_pxpy = atan2(py, px);

  VectorXd z_pred(3);
  z_pred << sqrt_pxpy2, arctan_pxpy, ((px*vx + py*vy)/sqrt_pxpy2);
  VectorXd y = z - z_pred;

  // Check to keep the phi angle within the range of -pi to pi. 
  double phi = y(1);
  double lower_pi = -1.0 * PI;
  double upper_pi = PI;

  while (phi > upper_pi  || phi < lower_pi) {
    if ( phi > upper_pi)
      phi = phi - (2.0 * PI);
    else if (phi < lower_pi)
      phi = phi + (2.0 * PI);
  }
  y(1) = phi;
  
  MatrixXd Ht = H_.transpose();
  MatrixXd S = H_ * P_ * Ht + R_;
  MatrixXd Si = S.inverse();
  MatrixXd PHt = P_ * Ht;
  MatrixXd K = PHt * Si;

  // new estimate
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_) * P_; 

}

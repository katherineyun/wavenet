#include <iostream>
// #include "ap_fixed.h"
// typedef ap_fixed<16,6> data_t;

typedef float data_t;

// unit-stride, odd or even kernels
// support same padding
void Conv1D(
  data_t* weight, data_t* input, data_t* output, 
  int in_len, int out_len, int ker_len, int in_chan, int out_chan, int dil_rate,
  int relu, int pad
){
  for(int m = 0; m < out_chan; m++){
    for(int o = 0; o < out_len; o++){
      data_t accum = 0;
      for(int c = 0; c < in_chan; c++){
        for(int k = 0; k < ker_len; k++){
          in_idx = ;
          if (in_idx >= 0 && in_idx < in_len )
            accum += input[c*in_len + in_idx]*weight[m*(in_chan*ker_len) + c*ker_len + k];
        }
      }
      if (relu == 0 && accum >= 0)
        output[m*out_len + o]= accum;
      else
        output[m*out_len + o]= 0;
    }
  }
}

void tanh(){

}

void sigmoid(){

}

void relu(data_t* input, data_t* output, int data_len){
  for(int i = 0; i < data_len; i++){
    if (input[i] >= 0)
      output[i] = input[i];
    else
      output[i] = 0;
  }
}


void submodule(data_t* input, data_t* output){


}
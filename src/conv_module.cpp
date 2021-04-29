#include <iostream>
#include <math.h>
// #include <hls_math.h>
// #include "ap_fixed.h"
// typedef ap_fixed<16,6> data_t;

#define DATA_LEN 8//4096
#define INPUT_CHAN 2
typedef float data_t;
typedef float table_t;

// unit-stride, odd or even kernels
// support 0 padding valid, 1 padding same
// void Conv1D(
//   data_t* weight, data_t* input, data_t* output, 
//   int in_len, int out_len, int ker_len, int in_chan, int out_chan, int dil_rate,
//   int relu, int pad
// ){
//   for(int m = 0; m < out_chan; m++){
//     for(int o = 0; o < out_len; o++){
//       data_t accum = 0;
//       for(int c = 0; c < in_chan; c++){
//         for(int k = 0; k < ker_len; k++){
//           int in_idx;
//           if (pad == 0)
//             in_idx = o + k*dil_rate;
//           else
//             in_idx = o + (k - (ker_len-1)/2)*dil_rate;

//           if (in_idx >= 0 && in_idx < in_len)
//             accum += input[c*in_len + in_idx]*weight[m*(in_chan*ker_len) + c*ker_len + k];
//         }
//       }
//       if (relu == 0 && accum >= 0)
//         output[m*out_len + o]= accum;
//       else
//         output[m*out_len + o]= 0;
//     }
//   }
// }

// store pre-computed in LUT, -4~4

void tanh_init(table_t* table, int tab_size){
  float val;
  for(int i = 0; i < tab_size; i++){
    val = 2*4.0*i/tab_size - 4.0;
    table_t res = tanh(val);
    table[i] = val;
  }
}

void tanh(data_t* input, data_t* output, int data_len, int in_chan, int tab_size){
  table_t table[tab_size];
  tanh_init(table, tab_size);
  for(int i = 0; i < in_chan; i++){
    for(int j = 0; j < data_len; j++){
      int idx = (input[i*data_len + j] + 4)*tab_size/8;
      idx = std::max(0, idx); // find nearest idx
      if (idx >= tab_size) idx = tab_size - 1;
      output[i*data_len + j] = (data_t) table[idx];
      
    }
  }
}

void sigmoid_init(table_t* table, int tab_size){

}

void sigmoid(data_t* input, data_t* output, int data_len, int in_chan){

}

void relu(data_t* input, data_t* output, int data_len, int in_chan){
  for(int i = 0; i < in_chan; i++){
    for(int j = 0; j < data_len; j++){
      if (input[i*data_len + j] >= 0)
        output[i*data_len + j] = input[i*data_len + j];
      else
        output[i*data_len + j] = 0;
    }
  }
}

void mult(data_t* A, data_t* B, data_t* C, int data_len, int in_chan){
  for(int i = 0; i < in_chan; i++){
    for(int j = 0; j < data_len; j++){
      C[i*data_len + j] = A[i*data_len + j] * B[i*data_len + j];
    }
  }
}

void add(data_t* A, data_t* B, data_t* C, int data_len, int in_chan){
  for(int i = 0; i < in_chan; i++){
    for(int j = 0; j < data_len; j++){
      C[i*data_len + j] = A[i*data_len + j] + B[i*data_len + j];
    }
  }
}

/**
concatenate two matrices of same size, channel-wise (axis = 1) 
or vector-wise (axis = 0). 
*/
void concate(data_t* A, data_t* B, data_t* C, int axis, int in_chan, int data_len){
  if (axis == 0){
      for(int i = 0; i < in_chan; i++){
        for(int j = 0; j < data_len; j++){
          C[i*data_len*2 + j] = A[i*data_len + j];
          C[i*data_len*2 + data_len + j] = B[i*data_len + j];
        }
      }
  }
  else{
      for(int i = 0; i < in_chan; i++){
        for(int j = 0; j < data_len; j++){
          C[i*data_len + j] = A[i*data_len + j];
          C[(i + in_chan)*data_len + j] = B[i*data_len + j];
        }
      }
  }
}

// void submodule(data_t* input, data_t* weight, data_t* output, int data_len){
//   int pad = 1; //'same'
//   data_t l1_pre[data_len], l1_f[data_len*16], l1_g, l1_z, l1_post, l1_res;

//   data_t layer_stack[data_len*16];
//   // (4096, 1) -> conv1d(16) -> (4096, 16)
//   data_t w1_pre[], w1_f[], w1_g[], w1_z[], w1_post[], w1_res[];

//   for (int iter = 0; iter < 3; iter++){
//     for (int exp = 0; exp < 11; exp++){
//       int dil_rate = pow(2, exp);

//       // Conv1D(weight, input, output, in_len, out_len, ker_len, in_chan, out_chan, dil_rate, relu, pad);

//       Conv1D(w1_pre, input, l1_pre, data_len, data_len, 1, 1, 16, 1, 1, pad); // preprocess (4096x1)
//       Conv1D(w1_f, l1_pre, l1_f, data_len, data_len, 2, 16, 32, dil_rate, 0, pad);
//       Conv1D(w1_g, l1_pre, l1_g, data_len, data_len, 2, 16, 32, dil_rate, 0, pad);
      
//       //multiply filter and gate branches
//       tanh(l1_f, l1_tan);
//       sigmoid(l1_g, l1_sig);
//       mult(l1_tan, l1_sig, l1_mult, data_len, 32);

//       //postprocess (4096, 16)
//       Conv1D(w1_post, l1_mult, l1_post, data_len, data_len, 1, 32, 16, 1, 1, pad);
//       //skip connection
//       add(l1_post, l1_pre, l1_add, data_len, 16);

//       for (int c = 0; c < 16; c++){
//         for (int i  = 0; i < data_len; i++){
//           layer_stack[c*data_len + i] += l1_add[c*data_len + i];
//         }
//       }
      
//     }
//   }
//   relu(layer_stack, output, data_len, 16);
// }



// void full_module(data_t* input, data_t* output, data_t* weight){
//   data_t x_a[DATA_LEN], x_b[DATA_LEN];
//   // reshape input (2x4096)
//   for(int i = 0; i < DATA_LEN; i++){
//     x_a[i] = input[i];
//     x_b[i] = input[DATA_LEN + i];
//   }

//   // pass submodules
//   submodule(x_a, w_sub1, sub_a, DATA_LEN);
//   submodule(x_b, w_sub2, sub_b, DATA_LEN);

//   // concatenate submodule outputs
//   concate(sub_a, sub_b, axis, x_conca);

//   // leaf layers
//   Conv1D(w_conv1, x_conca, x_conv1_o, 4096, 4096, 1, 32, 128, 1, 1, 1);
//   Conv1D(w_conv1, x_conv1_o, x_conv2_o, 4096, 4096, 1, 128, 1, 1, 0, 1);

//   sigmoid(x_conv2_o, output, DATA_LEN, in_chan);
// }

int main(){
  data_t input[8] = {1.0,2.0,3.0,4.0,5.0,6.0,7.0,-8.0};
  data_t x_a[4];
  data_t x_b[4];

  return 0;
}
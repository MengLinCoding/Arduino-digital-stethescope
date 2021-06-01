// adds new value to start of array, while deleting end of array
// array size is constant
void pushPop(float newValue, float *arr, int arraySize)
{
  for(int i = arraySize-1; i > 0; i--){
    arr[i] = arr[i-1];
  }
  arr[0] = newValue;
}




float variance(float *arr, int arraySize)
{
  float ave = mean(arr, arraySize);
  float output = 0;
  for(int i = 0; i < arraySize; i++){
    output += pow( arr[i] - ave , 2 );
  }
  return output/arraySize;
}





float mean(float *arr, int arraySize)
{
  float output = 0;
  for(int i = 0; i < arraySize; i++){
    output += arr[i];
  }
  return output/arraySize;
}

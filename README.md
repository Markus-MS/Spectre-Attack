# Spectre-Attack
Spectre uses Speculative execution to execute branches that would otherwise due to security checks not execute.  

There are two different Spectre variants. This repository contains a simple C Program with a Spectre Variant 1 attack implementation. 

The target of this program is to access the secret data in an array. But it only has access to the boring part. But with speculative execution and cache timing analysis as well as miss training the branch predictor (the Branch History Table to be more precise), we can get access to the secret data.   
## Build
```shell
$ make
$ ./spectre
```

## More Information 
For more details on how Spectre works: [Spectre Paper](https://spectreattack.com/spectre.pdf)

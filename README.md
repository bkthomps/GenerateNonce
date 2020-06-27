# GenerateNonce
GenerateNonce

The `libXKCP.a` and `main` in this repository are for Ivy Bridge on my machine (Mid 2012 13-inch MacBook Pro).

If your machine is different:
1. Clone the `https://github.com/XKCP/XKCP` repository
2. Figure our your processor instruction set, Sandy Bridge and Ivy Bridge are AVX, and Haswell and Skylake are AVX2.
3. Go into that cloned repository and run `AVX/libXKCP.a` for a static library. AVX can be replaced with any supported processor instruction set.
4. Copy over the new `libXKCP.a` to this repository.
5. Run `clang++ main.cpp -o main libXKCP.a -ldl -std=gnu++17`
6. Then you can run `./main` to run it. On my machine (Mid 2012 13-inch MacBook Pro) it processes 500 000 nonces per second.
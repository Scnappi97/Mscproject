#include "dsa.h"
#include "osrng.h"

int main(int argc, char* argv[])
{
   AutoSeededRandomPool rng;

   // Generate Private Key
   DSA::PrivateKey privateKey;
   privateKey.GenerateRandomWithKeySize(rng, 2048);
   
   // Generate Public Key   
   DSA::PublicKey publicKey;
   publicKey.AssignFrom(PrivateKey);
   if (!privateKey.Validate(rng, 3) || !publicKey.Validate(rng, 3))
   {
      throw runtime_error("DSA key generation failed");
   }

   return 0;
}
Some notes on the design:

1) I did not implement the factory pattern to create the data-container objects
   since the classes defined here will be used as based classes of the real
   classes that will be used by client code. 
   
   If the idea of generating the classes using the metadata of the container is
   implemented, you could easily find yourself with hundreds of generated classes.
   Evidently, the factory pattern makes no sense in such a situation.
   
2) To make the API easier to read, most classes of Photon were split in two.
   The private data and the definition of the C API calls are "hidden" in the 
   files Private*.cs.
   

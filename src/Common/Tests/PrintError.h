
void printError( vdscErrorHandler* pError )
{
   char* msg = NULL;
   size_t length = 0;

   if ( pError == NULL )
      fprintf( stderr, "Null ErrorHandler...can't help you!\n" );
   else
   {
      if ( vdscAnyErrors( pError ) )
      {
         length = vdscGetErrorMsgLength( pError );
         if ( length == 0 )
            fprintf( stderr, "Error in errorHandler... a bit ironic!\n" );
         else
         {
            msg = (char*)malloc( length+1 );
            if ( msg == NULL )
            {
               fprintf( stderr, "Malloc error in printError\n" );
            }
            else
            {
               vdscGetErrorMsg( pError, msg, length+1 );
               fprintf( stderr, "Error message: %s\n", msg );
            }
         }
      }
   }
   if ( msg != NULL ) free(msg);
}

/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

#include "info.pb-c.h"
void   pso__info__init
                     (Pso__Info         *message)
{
  static Pso__Info init_value = PSO__INFO__INIT;
  *message = init_value;
}
size_t pso__info__get_packed_size
                     (const Pso__Info *message)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &pso__info__descriptor);
  return protobuf_c_message_get_packed_size ((const ProtobufCMessage*)(message));
}
size_t pso__info__pack
                     (const Pso__Info *message,
                      uint8_t       *out)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &pso__info__descriptor);
  return protobuf_c_message_pack ((const ProtobufCMessage*)message, out);
}
size_t pso__info__pack_to_buffer
                     (const Pso__Info *message,
                      ProtobufCBuffer *buffer)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &pso__info__descriptor);
  return protobuf_c_message_pack_to_buffer ((const ProtobufCMessage*)message, buffer);
}
Pso__Info *
       pso__info__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data)
{
  return (Pso__Info *)
     protobuf_c_message_unpack (&pso__info__descriptor,
                                allocator, len, data);
}
void   pso__info__free_unpacked
                     (Pso__Info *message,
                      ProtobufCAllocator *allocator)
{
  PROTOBUF_C_ASSERT (message->base.descriptor == &pso__info__descriptor);
  protobuf_c_message_free_unpacked ((ProtobufCMessage*)message, allocator);
}
static const ProtobufCFieldDescriptor pso__info__field_descriptors[15] =
{
  {
    "total_size_in_bytes",
    1,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT64,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Pso__Info, total_size_in_bytes),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "allocated_size_in_bytes",
    2,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT64,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Pso__Info, allocated_size_in_bytes),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "num_objects",
    3,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT64,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Pso__Info, num_objects),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "num_groups",
    4,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT64,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Pso__Info, num_groups),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "num_mallocs",
    5,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_UINT64,
    PROTOBUF_C_OFFSETOF(Pso__Info, has_num_mallocs),
    PROTOBUF_C_OFFSETOF(Pso__Info, num_mallocs),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "num_frees",
    6,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_UINT64,
    PROTOBUF_C_OFFSETOF(Pso__Info, has_num_frees),
    PROTOBUF_C_OFFSETOF(Pso__Info, num_frees),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "largest_free_in_bytes",
    7,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_UINT64,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Pso__Info, largest_free_in_bytes),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "memory_version",
    8,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_INT32,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Pso__Info, memory_version),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "big_endian",
    9,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_BOOL,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Pso__Info, big_endian),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "compiler",
    10,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Pso__Info, compiler),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "compiler_version",
    11,
    PROTOBUF_C_LABEL_OPTIONAL,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Pso__Info, compiler_version),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "platform",
    12,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Pso__Info, platform),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "dll_version",
    13,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Pso__Info, dll_version),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "quasar_version",
    14,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Pso__Info, quasar_version),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
  {
    "creation_time",
    15,
    PROTOBUF_C_LABEL_REQUIRED,
    PROTOBUF_C_TYPE_STRING,
    0,   /* quantifier_offset */
    PROTOBUF_C_OFFSETOF(Pso__Info, creation_time),
    NULL,
    NULL,
    NULL,NULL    /* reserved1, reserved2 */
  },
};
static const unsigned pso__info__field_indices_by_name[] = {
  1,   /* field[1] = allocated_size_in_bytes */
  8,   /* field[8] = big_endian */
  9,   /* field[9] = compiler */
  10,   /* field[10] = compiler_version */
  14,   /* field[14] = creation_time */
  12,   /* field[12] = dll_version */
  6,   /* field[6] = largest_free_in_bytes */
  7,   /* field[7] = memory_version */
  5,   /* field[5] = num_frees */
  3,   /* field[3] = num_groups */
  4,   /* field[4] = num_mallocs */
  2,   /* field[2] = num_objects */
  11,   /* field[11] = platform */
  13,   /* field[13] = quasar_version */
  0,   /* field[0] = total_size_in_bytes */
};
static const ProtobufCIntRange pso__info__number_ranges[1 + 1] =
{
  { 1, 0 },
  { 0, 15 }
};
const ProtobufCMessageDescriptor pso__info__descriptor =
{
  PROTOBUF_C_MESSAGE_DESCRIPTOR_MAGIC,
  "pso.Info",
  "Info",
  "Pso__Info",
  "pso",
  sizeof(Pso__Info),
  15,
  pso__info__field_descriptors,
  pso__info__field_indices_by_name,
  1,  pso__info__number_ranges,
  NULL,NULL,NULL,NULL    /* reserved[1234] */
};

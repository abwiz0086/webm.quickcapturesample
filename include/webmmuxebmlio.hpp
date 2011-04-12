// Copyright (c) 2010 The WebM project authors. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS.  All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.

#ifndef WEBMUXEBMLIO_HPP
#define WEBMUXEBMLIO_HPP

namespace webmmux
{
enum EbmlIOSeekPosition
{
  EBMLIO_SEEK_BEGINNING,
  EBMLIO_SEEK_CURRENT,
  EBMLIO_SEEK_END
};

class EbmlStreamInterface
{
protected:
  virtual ~EbmlStreamInterface();

public:
  // Seeks to offset from startPos.
  // Value of offset may be past end of stream, which should
  // produce an automatic expansion of stream.
  // The new stream pointer position is copied to newPos.
  // Returns zero success.
  virtual int Seek(long long offset, EbmlIOSeekPosition startPos,
                   unsigned long long* newPos) = 0;

  // Writes bytesToWrite bytes from buf to stream.
  // Returns zero success.
  virtual int Write(void const* buf, unsigned long long bytesToWrite) = 0;

  // Reads bytesToRead bytes from stream into buf.
  // Returns zero success.
  virtual int Read(void* buf, unsigned long bytesToRead) = 0;
};


class File
{
  File(const File&);
  File& operator=(const File&);

public:

  File();
  ~File();

  void SetStream(EbmlStreamInterface* p);
  EbmlStreamInterface* GetStream() const;

  long long SetPosition(long long,
                        EbmlIOSeekPosition = EBMLIO_SEEK_BEGINNING);
  long long GetPosition() const;

  void Write(const void*, unsigned long);
  void WriteBigEndOrder(const void* data_ptr, const int data_size);

  void WriteIDHeader(const unsigned long id);
  // WriteDataSizeHeader adds the EBML size header bits.
  // When force_size_bytes > 0, it writes that many bytes, even if they are
  // not needed.
  void WriteDataSizeHeader(const unsigned long long data_size,
                           const int force_size_bytes = 0);
  // RewriteDataSizeHeader will overwrite a data size header at a given
  // existing position in the stream.  It calculates the size of the header
  // from the existing value, so it is important that a valid value already
  // exist at that position.  The current stream postion is restored
  // on return.
  void RewriteDataSizeHeader(const long long data_size_header_position,
                             const unsigned long long data_size);

  void WriteElementSignedInt(const unsigned long id,
                             const signed long long value);
  void WriteElementUnsignedInt(const unsigned long id,
                               const unsigned long long value);
  void WriteElementFloat4(const unsigned long id, const float value);
  void WriteElementString(const unsigned long id, const char *buffer);
  void WriteElementUTF8String(const unsigned long id,
                              const wchar_t *wide_buffer);
  void WriteElementBinary(const unsigned long id, const void *buffer,
                          const long long length);
  // Returns zero success.
  int Read(void* buffer, const unsigned long bytes);

private:
  EbmlStreamInterface* m_pStream;
};

// Codes the EBML lead bit that indicates how many bytes the value takes.
unsigned long long CodeEBMLSizeBit(const unsigned long long);

// MostSignificantBitPosition finds the zsero-based position of the most
// significant bit in the value.  This is useful for writing EBML values that
// take up less space.
int MostSignificantBitPosition(const unsigned long long);

unsigned long long SetPosition(EbmlStreamInterface *pStream, long long,
                               EbmlIOSeekPosition);

// The WriteBigEndOrder functions write the data in the supplied buffer in
// "big-endian order" or "network byte order."
void WriteBigEndOrder(EbmlStreamInterface *pStream, const unsigned char*,
                      const unsigned char*);
void WriteBigEndOrder(EbmlStreamInterface *pStream, const void*, const int);

// WriteBigEndOrderUnsignedInt does the network order swap for several of the
// WriteElementX functions.  It writes the value in as few bytes as possible.
void WriteBigEndOrderUnsignedInt(EbmlStreamInterface *pStream,
                                 const unsigned long long value);
void Write(EbmlStreamInterface *pStream, const void*, unsigned long long);

void WriteIDHeader(EbmlStreamInterface *pStream, const unsigned long id);
void WriteDataSizeHeader(EbmlStreamInterface *pStream,
                         const unsigned long long data_size,
                         const int force_size_bytes = 0);
void RewriteDataSizeHeader(EbmlStreamInterface *pStream,
                           const long long data_size_header_position,
                           const unsigned long long data_size);
void WriteElementSignedInt(EbmlStreamInterface *pStream,
                           const unsigned long id,
                           const signed long long value);
void WriteElementUnsignedInt(EbmlStreamInterface *pStream,
                             const unsigned long id,
                             const unsigned long long value);
void WriteElementFloat4(EbmlStreamInterface *pStream, const unsigned long id,
                        const float value);
void WriteElementString(EbmlStreamInterface *pStream, const unsigned long id,
                        const char * buffer);
void WriteElementUTF8String(EbmlStreamInterface *pStream,
                            const unsigned long id, const wchar_t * buffer);
void WriteElementBinary(EbmlStreamInterface *pStream, const unsigned long id,
                        const void *buffer, const long long length);

int Read(EbmlStreamInterface *pStream, void * buffer,
         const unsigned long bytes);

}  // namespace webmmux

#endif  // WEBMUXEBMLIO_HPP

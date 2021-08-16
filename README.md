# libqrencode

Libqrencode is a library for encoding data in a QR Code symbol, a kind of 2D
symbology.

Adaption of libqrencode under stewardship of Kentaro Fukuchi. Original source can
be found at https://github.com/fukuchi/libqrencode

## License

Re-licensed under LGPL 3.0 from previous 2.1 as permitted by original author.

## Changeset overview

Added qrprint module, defining the function *QRprint_utf8*, for convenience.
Namespaced masking data.

### Usage example

```c
#include <stdlib.h>
#include <stdio.h>
#include <qrprint.h>

int main(void)
{
    const char* text = "https://think-biq.com";
    // Learn more about quality and version at https://www.qrcode.com/en/about/
    QRecLevel quality = QR_ECLEVEL_L;
    int version = 0;
    int casesensitive = 1;

    // Prints a UTF8 encoded QR code to stdout.
    QRprint_utf8(stdout, text, quality, version, casesensitive);

    return 0;
}

```

## Encoding

There are two methods to encode data: <b>encoding a string/data</b> or
<b>encoding a structured data</b>.

### Encoding a string/data
You can encode a string by calling QRcode_encodeString().
The given string is parsed automatically and encoded. If you want to encode
data that can be represented as a C string style (NUL terminated), you can
simply use this way.

If the input data contains Kanji (Shift-JIS) characters and you want to
encode them as Kanji in QR Code, you should give QR_MODE_KANJI as a hint.
Otherwise, all of non-alphanumeric characters are encoded as 8-bit data.
If you want to encode a whole string in 8-bit mode, you can use
QRcode_encodeString8bit() instead.

Please note that a C string can not contain NUL characters. If your data
contains NUL, you must use QRcode_encodeData().

### Encoding a structured data
You can construct a structured input data manually. If the structure of the
input data is known, you can use this method.
At first, create a ::QRinput object by QRinput_new(). Then add input data
to the QRinput object by QRinput_append(). Finally call QRcode_encodeInput()
to encode the QRinput data.
You can reuse the QRinput object again to encode it in other symbols with
different parameters.

### Result
The encoded symbol is generated as a ::QRcode object. It will contain its
version number, the width of the symbol, and an array represents the symbol.
See ::QRcode for the details. You can free the object by QRcode_free().

Please note that the version of the result may be larger than specified.
In such cases, the input data would be too large to be encoded in a
symbol of the specified version.

### Structured append
Libqrencode can generate "Structured-appended" symbols that enables to split
a large data set into mulitple QR codes. A QR code reader concatenates
multiple QR code symbols into a string.
Just like QRcode_encodeString(), you can use QRcode_encodeStringStructured()
to generate structured-appended symbols. This functions returns an instance
of ::QRcode_List. The returned list is a singly-linked list of QRcode: you
can retrieve each QR code in this way:

```c
QRcode_List *qrcodes;
QRcode_List *entry;
QRcode *qrcode;

qrcodes = QRcode_encodeStringStructured(...);
entry = qrcodes;
while(entry != NULL) {
 qrcode = entry->code;
 // do something
 entry = entry->next;
}
QRcode_List_free(entry);
```

Instead of using auto-parsing functions, you can construct your own
structured input. At first, instantiate an object of ::QRinput_Struct
by calling QRinput_Struct_new(). This object can hold multiple ::QRinput,
and one QR code is generated for a ::QRinput.
QRinput_Struct_appendInput() appends a ::QRinput to a ::QRinput_Struct
object. In order to generate structured-appended symbols, it is required to
embed headers to each symbol. You can use
QRinput_Struct_insertStructuredAppendHeaders() to insert appropriate
headers to each symbol. You should call this function just once before
encoding symbols.
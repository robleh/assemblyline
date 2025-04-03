SECTIONS
{
  . = SIZEOF_HEADERS;
  . = ALIGN(__section_alignment__);
  .pic  __image_base__ + ( __section_alignment__ < 0x1000 ? . : __section_alignment__ ) :
  {
    *(.al$*)
    *(.rdata)
    *(SORT(.rdata$*))
    *(.data)
    *(SORT(.data$*))
    *(.text)
    *(SORT(.text$*))
  }
}

INCLUDE dwarf.ld;

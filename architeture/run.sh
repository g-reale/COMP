  iverilog -o processor_audio_tb.vvp processor_audio_tb.v \
           processor.v alu.v ram.v ssd.v lcd.v audio.v circular.v rising.v \
           global.v \
  && vvp processor_audio_tb.vvp > log.txt
  
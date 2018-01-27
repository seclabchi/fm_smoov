##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=audiod
ConfigurationName      :=Debug
WorkspacePath          := "/opt/repos/fm_smoov"
ProjectPath            := "/opt/repos/fm_smoov"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Paul Zaremba
Date                   :=01/26/18
CodeLitePath           :=""
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="audiod.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)/usr/include/alsa $(IncludeSwitch)hdr 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)asound $(LibrarySwitch)pthread $(LibrarySwitch)sndfile 
ArLibs                 :=  "asound" "pthread" "sndfile" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)/usr/lib/arm-linux-gnueabihf 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall -std=c++11 $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
Objects0=$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_device_db.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_pcm_device.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_pcm_playback.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_pcm_capture.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_audio_hub.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_processor_simple_gain.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_tone_generator.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_processor_analyzer.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_logger.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_processor.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_biquad.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_command_handler.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_stream_capture.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_soundfile.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

PostBuild:
	@echo Executing Post Build commands ...
	
	@echo Done

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp $(IntermediateDirectory)/src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(DependSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM "src/main.cpp"

$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix) "src/main.cpp"

$(IntermediateDirectory)/src_device_db.cpp$(ObjectSuffix): src/device_db.cpp $(IntermediateDirectory)/src_device_db.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/device_db.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_device_db.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_device_db.cpp$(DependSuffix): src/device_db.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_device_db.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_device_db.cpp$(DependSuffix) -MM "src/device_db.cpp"

$(IntermediateDirectory)/src_device_db.cpp$(PreprocessSuffix): src/device_db.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_device_db.cpp$(PreprocessSuffix) "src/device_db.cpp"

$(IntermediateDirectory)/src_pcm_device.cpp$(ObjectSuffix): src/pcm_device.cpp $(IntermediateDirectory)/src_pcm_device.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/pcm_device.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_pcm_device.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_pcm_device.cpp$(DependSuffix): src/pcm_device.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_pcm_device.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_pcm_device.cpp$(DependSuffix) -MM "src/pcm_device.cpp"

$(IntermediateDirectory)/src_pcm_device.cpp$(PreprocessSuffix): src/pcm_device.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_pcm_device.cpp$(PreprocessSuffix) "src/pcm_device.cpp"

$(IntermediateDirectory)/src_pcm_playback.cpp$(ObjectSuffix): src/pcm_playback.cpp $(IntermediateDirectory)/src_pcm_playback.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/pcm_playback.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_pcm_playback.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_pcm_playback.cpp$(DependSuffix): src/pcm_playback.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_pcm_playback.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_pcm_playback.cpp$(DependSuffix) -MM "src/pcm_playback.cpp"

$(IntermediateDirectory)/src_pcm_playback.cpp$(PreprocessSuffix): src/pcm_playback.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_pcm_playback.cpp$(PreprocessSuffix) "src/pcm_playback.cpp"

$(IntermediateDirectory)/src_pcm_capture.cpp$(ObjectSuffix): src/pcm_capture.cpp $(IntermediateDirectory)/src_pcm_capture.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/pcm_capture.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_pcm_capture.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_pcm_capture.cpp$(DependSuffix): src/pcm_capture.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_pcm_capture.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_pcm_capture.cpp$(DependSuffix) -MM "src/pcm_capture.cpp"

$(IntermediateDirectory)/src_pcm_capture.cpp$(PreprocessSuffix): src/pcm_capture.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_pcm_capture.cpp$(PreprocessSuffix) "src/pcm_capture.cpp"

$(IntermediateDirectory)/src_audio_hub.cpp$(ObjectSuffix): src/audio_hub.cpp $(IntermediateDirectory)/src_audio_hub.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/audio_hub.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_audio_hub.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_audio_hub.cpp$(DependSuffix): src/audio_hub.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_audio_hub.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_audio_hub.cpp$(DependSuffix) -MM "src/audio_hub.cpp"

$(IntermediateDirectory)/src_audio_hub.cpp$(PreprocessSuffix): src/audio_hub.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_audio_hub.cpp$(PreprocessSuffix) "src/audio_hub.cpp"

$(IntermediateDirectory)/src_processor_simple_gain.cpp$(ObjectSuffix): src/processor_simple_gain.cpp $(IntermediateDirectory)/src_processor_simple_gain.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/processor_simple_gain.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_processor_simple_gain.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_processor_simple_gain.cpp$(DependSuffix): src/processor_simple_gain.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_processor_simple_gain.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_processor_simple_gain.cpp$(DependSuffix) -MM "src/processor_simple_gain.cpp"

$(IntermediateDirectory)/src_processor_simple_gain.cpp$(PreprocessSuffix): src/processor_simple_gain.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_processor_simple_gain.cpp$(PreprocessSuffix) "src/processor_simple_gain.cpp"

$(IntermediateDirectory)/src_tone_generator.cpp$(ObjectSuffix): src/tone_generator.cpp $(IntermediateDirectory)/src_tone_generator.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/tone_generator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_tone_generator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_tone_generator.cpp$(DependSuffix): src/tone_generator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_tone_generator.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_tone_generator.cpp$(DependSuffix) -MM "src/tone_generator.cpp"

$(IntermediateDirectory)/src_tone_generator.cpp$(PreprocessSuffix): src/tone_generator.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_tone_generator.cpp$(PreprocessSuffix) "src/tone_generator.cpp"

$(IntermediateDirectory)/src_processor_analyzer.cpp$(ObjectSuffix): src/processor_analyzer.cpp $(IntermediateDirectory)/src_processor_analyzer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/processor_analyzer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_processor_analyzer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_processor_analyzer.cpp$(DependSuffix): src/processor_analyzer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_processor_analyzer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_processor_analyzer.cpp$(DependSuffix) -MM "src/processor_analyzer.cpp"

$(IntermediateDirectory)/src_processor_analyzer.cpp$(PreprocessSuffix): src/processor_analyzer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_processor_analyzer.cpp$(PreprocessSuffix) "src/processor_analyzer.cpp"

$(IntermediateDirectory)/src_logger.cpp$(ObjectSuffix): src/logger.cpp $(IntermediateDirectory)/src_logger.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/logger.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_logger.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_logger.cpp$(DependSuffix): src/logger.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_logger.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_logger.cpp$(DependSuffix) -MM "src/logger.cpp"

$(IntermediateDirectory)/src_logger.cpp$(PreprocessSuffix): src/logger.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_logger.cpp$(PreprocessSuffix) "src/logger.cpp"

$(IntermediateDirectory)/src_processor.cpp$(ObjectSuffix): src/processor.cpp $(IntermediateDirectory)/src_processor.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/processor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_processor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_processor.cpp$(DependSuffix): src/processor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_processor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_processor.cpp$(DependSuffix) -MM "src/processor.cpp"

$(IntermediateDirectory)/src_processor.cpp$(PreprocessSuffix): src/processor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_processor.cpp$(PreprocessSuffix) "src/processor.cpp"

$(IntermediateDirectory)/src_biquad.cpp$(ObjectSuffix): src/biquad.cpp $(IntermediateDirectory)/src_biquad.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/biquad.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_biquad.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_biquad.cpp$(DependSuffix): src/biquad.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_biquad.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_biquad.cpp$(DependSuffix) -MM "src/biquad.cpp"

$(IntermediateDirectory)/src_biquad.cpp$(PreprocessSuffix): src/biquad.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_biquad.cpp$(PreprocessSuffix) "src/biquad.cpp"

$(IntermediateDirectory)/src_command_handler.cpp$(ObjectSuffix): src/command_handler.cpp $(IntermediateDirectory)/src_command_handler.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/command_handler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_command_handler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_command_handler.cpp$(DependSuffix): src/command_handler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_command_handler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_command_handler.cpp$(DependSuffix) -MM "src/command_handler.cpp"

$(IntermediateDirectory)/src_command_handler.cpp$(PreprocessSuffix): src/command_handler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_command_handler.cpp$(PreprocessSuffix) "src/command_handler.cpp"

$(IntermediateDirectory)/src_stream_capture.cpp$(ObjectSuffix): src/stream_capture.cpp $(IntermediateDirectory)/src_stream_capture.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/stream_capture.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_stream_capture.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_stream_capture.cpp$(DependSuffix): src/stream_capture.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_stream_capture.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_stream_capture.cpp$(DependSuffix) -MM "src/stream_capture.cpp"

$(IntermediateDirectory)/src_stream_capture.cpp$(PreprocessSuffix): src/stream_capture.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_stream_capture.cpp$(PreprocessSuffix) "src/stream_capture.cpp"

$(IntermediateDirectory)/src_soundfile.cpp$(ObjectSuffix): src/soundfile.cpp $(IntermediateDirectory)/src_soundfile.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/opt/repos/fm_smoov/src/soundfile.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_soundfile.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_soundfile.cpp$(DependSuffix): src/soundfile.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_soundfile.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_soundfile.cpp$(DependSuffix) -MM "src/soundfile.cpp"

$(IntermediateDirectory)/src_soundfile.cpp$(PreprocessSuffix): src/soundfile.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_soundfile.cpp$(PreprocessSuffix) "src/soundfile.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/



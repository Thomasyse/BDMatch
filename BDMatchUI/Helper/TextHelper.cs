using Microsoft.UI.Xaml.Documents;
using Microsoft.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace BDMatchUI.Helper
{
    public class TextHelper : DependencyObject
    {
        // Dependency Properties for binding.

        // Generall
        public string OffText
        {
            get { return (string)GetValue(OffTextProperty); }
            set { SetValue(OffTextProperty, value); }
        }
        public static readonly DependencyProperty OffTextProperty =
            DependencyProperty.Register(nameof(OffText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string OnText
        {
            get { return (string)GetValue(OnTextProperty); }
            set { SetValue(OnTextProperty, value); }
        }
        public static readonly DependencyProperty OnTextProperty =
            DependencyProperty.Register(nameof(OnText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        // AboutPage

        public string AboutHeader
        {
            get { return (string)GetValue(AboutHeaderProperty); }
            set { SetValue(AboutHeaderProperty, value); }
        }
        public static readonly DependencyProperty AboutHeaderProperty =
            DependencyProperty.Register(nameof(AboutHeader), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string AboutText
        {
            get { return (string)GetValue(AboutTextProperty); }
            set { SetValue(AboutTextProperty, value); }
        }
        public static readonly DependencyProperty AboutTextProperty =
            DependencyProperty.Register(nameof(AboutText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        // DecodeSettingsPage

        public string DecodeSettingsText
        {
            get { return (string)GetValue(DecodeSettingsTextProperty); }
            set { SetValue(DecodeSettingsTextProperty, value); }
        }
        public static readonly DependencyProperty DecodeSettingsTextProperty =
            DependencyProperty.Register(nameof(DecodeSettingsText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string FFTNumText
        {
            get { return (string)GetValue(FFTNumTextProperty); }
            set { SetValue(FFTNumTextProperty, value); }
        }
        public static readonly DependencyProperty FFTNumTextProperty =
            DependencyProperty.Register(nameof(FFTNumText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string FFTNumElbr
        {
            get { return (string)GetValue(FFTNumElbrProperty); }
            set { SetValue(FFTNumElbrProperty, value); }
        }
        public static readonly DependencyProperty FFTNumElbrProperty =
            DependencyProperty.Register(nameof(FFTNumElbr), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string MinFindDBText
        {
            get { return (string)GetValue(MinFindDBTextProperty); }
            set { SetValue(MinFindDBTextProperty, value); }
        }
        public static readonly DependencyProperty MinFindDBTextProperty =
            DependencyProperty.Register(nameof(MinFindDBText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string DecibelText
        {
            get { return (string)GetValue(DecibelTextProperty); }
            set { SetValue(DecibelTextProperty, value); }
        }
        public static readonly DependencyProperty DecibelTextProperty =
            DependencyProperty.Register(nameof(DecibelText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string MinFindDBElbr
        {
            get { return (string)GetValue(MinFindDBElbrProperty); }
            set { SetValue(MinFindDBElbrProperty, value); }
        }
        public static readonly DependencyProperty MinFindDBElbrProperty =
            DependencyProperty.Register(nameof(MinFindDBElbr), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string OutputPCMText
        {
            get { return (string)GetValue(OutputPCMTextProperty); }
            set { SetValue(OutputPCMTextProperty, value); }
        }
        public static readonly DependencyProperty OutputPCMTextProperty =
            DependencyProperty.Register(nameof(OutputPCMText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string OutputPCMElbr
        {
            get { return (string)GetValue(OutputPCMElbrProperty); }
            set { SetValue(OutputPCMElbrProperty, value); }
        }
        public static readonly DependencyProperty OutputPCMElbrProperty =
            DependencyProperty.Register(nameof(OutputPCMElbr), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string ParallelDecodeText
        {
            get { return (string)GetValue(ParallelDecodeTextProperty); }
            set { SetValue(ParallelDecodeTextProperty, value); }
        }
        public static readonly DependencyProperty ParallelDecodeTextProperty =
            DependencyProperty.Register(nameof(ParallelDecodeText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string ParallelDecodeElbr
        {
            get { return (string)GetValue(ParallelDecodeElbrProperty); }
            set { SetValue(ParallelDecodeElbrProperty, value); }
        }
        public static readonly DependencyProperty ParallelDecodeElbrProperty =
            DependencyProperty.Register(nameof(ParallelDecodeElbr), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string VolMatchText
        {
            get { return (string)GetValue(VolMatchTextProperty); }
            set { SetValue(VolMatchTextProperty, value); }
        }
        public static readonly DependencyProperty VolMatchTextProperty =
            DependencyProperty.Register(nameof(VolMatchText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string VolMatchElbr
        {
            get { return (string)GetValue(VolMatchElbrProperty); }
            set { SetValue(VolMatchElbrProperty, value); }
        }
        public static readonly DependencyProperty VolMatchElbrProperty =
            DependencyProperty.Register(nameof(VolMatchElbr), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        // HelpPage

        public string HelpText
        {
            get { return (string)GetValue(HelpTextProperty); }
            set { SetValue(HelpTextProperty, value); }
        }
        public static readonly DependencyProperty HelpTextProperty =
            DependencyProperty.Register(nameof(HelpText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string BatchProcessingHeader
        {
            get { return (string)GetValue(BatchProcessingHeaderProperty); }
            set { SetValue(BatchProcessingHeaderProperty, value); }
        }
        public static readonly DependencyProperty BatchProcessingHeaderProperty =
            DependencyProperty.Register(nameof(BatchProcessingHeader), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string BatchProcessingText
        {
            get { return (string)GetValue(BatchProcessingTextProperty); }
            set { SetValue(BatchProcessingTextProperty, value); }
        }
        public static readonly DependencyProperty BatchProcessingTextProperty =
            DependencyProperty.Register(nameof(BatchProcessingText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string OneToMultiHeader
        {
            get { return (string)GetValue(OneToMultiHeaderProperty); }
            set { SetValue(OneToMultiHeaderProperty, value); }
        }
        public static readonly DependencyProperty OneToMultiHeaderProperty =
            DependencyProperty.Register(nameof(OneToMultiHeader), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string OneToMultiText
        {
            get { return (string)GetValue(OneToMultiTextProperty); }
            set { SetValue(OneToMultiTextProperty, value); }
        }
        public static readonly DependencyProperty OneToMultiTextProperty =
            DependencyProperty.Register(nameof(OneToMultiText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string UsingRegexHeader
        {
            get { return (string)GetValue(UsingRegexHeaderProperty); }
            set { SetValue(UsingRegexHeaderProperty, value); }
        }
        public static readonly DependencyProperty UsingRegexHeaderProperty =
            DependencyProperty.Register(nameof(UsingRegexHeader), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string UsingRegexText
        {
            get { return (string)GetValue(UsingRegexTextProperty); }
            set { SetValue(UsingRegexTextProperty, value); }
        }
        public static readonly DependencyProperty UsingRegexTextProperty =
            DependencyProperty.Register(nameof(UsingRegexText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string ExcludeSubExprHeader
        {
            get { return (string)GetValue(ExcludeSubExprHeaderProperty); }
            set { SetValue(ExcludeSubExprHeaderProperty, value); }
        }
        public static readonly DependencyProperty ExcludeSubExprHeaderProperty =
            DependencyProperty.Register(nameof(ExcludeSubExprHeader), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string ExcludeSubExprText
        {
            get { return (string)GetValue(ExcludeSubExprTextProperty); }
            set { SetValue(ExcludeSubExprTextProperty, value); }
        }
        public static readonly DependencyProperty ExcludeSubExprTextProperty =
            DependencyProperty.Register(nameof(ExcludeSubExprText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string FastRegexHeader
        {
            get { return (string)GetValue(FastRegexHeaderProperty); }
            set { SetValue(FastRegexHeaderProperty, value); }
        }
        public static readonly DependencyProperty FastRegexHeaderProperty =
            DependencyProperty.Register(nameof(FastRegexHeader), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string FastRegexText
        {
            get { return (string)GetValue(FastRegexTextProperty); }
            set { SetValue(FastRegexTextProperty, value); }
        }
        public static readonly DependencyProperty FastRegexTextProperty =
            DependencyProperty.Register(nameof(FastRegexText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        // LogsPage
        public string LogsText
        {
            get { return (string)GetValue(LogsTextProperty); }
            set { SetValue(LogsTextProperty, value); }
        }
        public static readonly DependencyProperty LogsTextProperty =
            DependencyProperty.Register(nameof(LogsText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        // MainWindow
        public string Match_and_Result_Header
        {
            get { return (string)GetValue(Match_and_Result_HeaderProperty); }
            set { SetValue(Match_and_Result_HeaderProperty, value); }
        }
        public static readonly DependencyProperty Match_and_Result_HeaderProperty =
            DependencyProperty.Register(nameof(Match_and_Result_Header), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string MatchPage_Item
        {
            get { return (string)GetValue(MatchPage_ItemProperty); }
            set { SetValue(MatchPage_ItemProperty, value); }
        }
        public static readonly DependencyProperty MatchPage_ItemProperty =
            DependencyProperty.Register(nameof(MatchPage_Item), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string LogsPage_Item
        {
            get { return (string)GetValue(LogsPage_ItemProperty); }
            set { SetValue(LogsPage_ItemProperty, value); }
        }
        public static readonly DependencyProperty LogsPage_ItemProperty =
            DependencyProperty.Register(nameof(LogsPage_Item), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string SpectrumPage_Item
        {
            get { return (string)GetValue(SpectrumPage_ItemProperty); }
            set { SetValue(SpectrumPage_ItemProperty, value); }
        }
        public static readonly DependencyProperty SpectrumPage_ItemProperty =
            DependencyProperty.Register(nameof(SpectrumPage_Item), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string Settings_Header
        {
            get { return (string)GetValue(Settings_HeaderProperty); }
            set { SetValue(Settings_HeaderProperty, value); }
        }
        public static readonly DependencyProperty Settings_HeaderProperty =
            DependencyProperty.Register(nameof(Settings_Header), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string MatchSettingsPage_Item
        {
            get { return (string)GetValue(MatchSettingsPage_ItemProperty); }
            set { SetValue(MatchSettingsPage_ItemProperty, value); }
        }
        public static readonly DependencyProperty MatchSettingsPage_ItemProperty =
            DependencyProperty.Register(nameof(MatchSettingsPage_Item), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string DecodeSettingsPage_Item
        {
            get { return (string)GetValue(DecodeSettingsPage_ItemProperty); }
            set { SetValue(DecodeSettingsPage_ItemProperty, value); }
        }
        public static readonly DependencyProperty DecodeSettingsPage_ItemProperty =
            DependencyProperty.Register(nameof(DecodeSettingsPage_Item), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string SpectrumSettingsPage_Item
        {
            get { return (string)GetValue(SpectrumSettingsPage_ItemProperty); }
            set { SetValue(SpectrumSettingsPage_ItemProperty, value); }
        }
        public static readonly DependencyProperty SpectrumSettingsPage_ItemProperty =
            DependencyProperty.Register(nameof(SpectrumSettingsPage_Item), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string OtherSettingsPage_Item
        {
            get { return (string)GetValue(OtherSettingsPage_ItemProperty); }
            set { SetValue(OtherSettingsPage_ItemProperty, value); }
        }
        public static readonly DependencyProperty OtherSettingsPage_ItemProperty =
            DependencyProperty.Register(nameof(OtherSettingsPage_Item), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string Others_Header
        {
            get { return (string)GetValue(Others_HeaderProperty); }
            set { SetValue(Others_HeaderProperty, value); }
        }
        public static readonly DependencyProperty Others_HeaderProperty =
            DependencyProperty.Register(nameof(Others_Header), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string HelpPage_Item
        {
            get { return (string)GetValue(HelpPage_ItemProperty); }
            set { SetValue(HelpPage_ItemProperty, value); }
        }
        public static readonly DependencyProperty HelpPage_ItemProperty =
            DependencyProperty.Register(nameof(HelpPage_Item), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string AboutPage_Item
        {
            get { return (string)GetValue(AboutPage_ItemProperty); }
            set { SetValue(AboutPage_ItemProperty, value); }
        }
        public static readonly DependencyProperty AboutPage_ItemProperty =
            DependencyProperty.Register(nameof(AboutPage_Item), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        // MatchPage
        public string SubPathPlaceholderText
        {
            get { return (string)GetValue(SubPathPlaceholderTextProperty); }
            set { SetValue(SubPathPlaceholderTextProperty, value); }
        }
        public static readonly DependencyProperty SubPathPlaceholderTextProperty =
            DependencyProperty.Register(nameof(SubPathPlaceholderText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string TVPathPlaceholderText
        {
            get { return (string)GetValue(TVPathPlaceholderTextProperty); }
            set { SetValue(TVPathPlaceholderTextProperty, value); }
        }
        public static readonly DependencyProperty TVPathPlaceholderTextProperty =
            DependencyProperty.Register(nameof(TVPathPlaceholderText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string BDPathPlaceholderText
        {
            get { return (string)GetValue(BDPathPlaceholderTextProperty); }
            set { SetValue(BDPathPlaceholderTextProperty, value); }
        }
        public static readonly DependencyProperty BDPathPlaceholderTextProperty =
            DependencyProperty.Register(nameof(BDPathPlaceholderText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string SubText
        {
            get { return (string)GetValue(SubTextProperty); }
            set { SetValue(SubTextProperty, value); }
        }
        public static readonly DependencyProperty SubTextProperty =
            DependencyProperty.Register(nameof(SubText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string TVText
        {
            get { return (string)GetValue(TVTextProperty); }
            set { SetValue(TVTextProperty, value); }
        }
        public static readonly DependencyProperty TVTextProperty =
            DependencyProperty.Register(nameof(TVText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string BDText
        {
            get { return (string)GetValue(BDTextProperty); }
            set { SetValue(BDTextProperty, value); }
        }
        public static readonly DependencyProperty BDTextProperty =
            DependencyProperty.Register(nameof(BDText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string MatchButtonContent
        {
            get { return (string)GetValue(MatchButtonContentProperty); }
            set { SetValue(MatchButtonContentProperty, value); }
        }
        public static readonly DependencyProperty MatchButtonContentProperty =
            DependencyProperty.Register(nameof(MatchButtonContent), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string MatchProgressText
        {
            get { return (string)GetValue(MatchProgressTextProperty); }
            set { SetValue(MatchProgressTextProperty, value); }
        }
        public static readonly DependencyProperty MatchProgressTextProperty =
            DependencyProperty.Register(nameof(MatchProgressText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string LogButtonContent
        {
            get { return (string)GetValue(LogButtonContentProperty); }
            set { SetValue(LogButtonContentProperty, value); }
        }
        public static readonly DependencyProperty LogButtonContentProperty =
            DependencyProperty.Register(nameof(LogButtonContent), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        // MatchSettingsPage
        public string MatchSettingsText
        {
            get { return (string)GetValue(MatchSettingsTextProperty); }
            set { SetValue(MatchSettingsTextProperty, value); }
        }
        public static readonly DependencyProperty MatchSettingsTextProperty =
            DependencyProperty.Register(nameof(MatchSettingsText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string MatchEnText
        {
            get { return (string)GetValue(MatchEnTextProperty); }
            set { SetValue(MatchEnTextProperty, value); }
        }
        public static readonly DependencyProperty MatchEnTextProperty =
            DependencyProperty.Register(nameof(MatchEnText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string MatchEnElbr
        {
            get { return (string)GetValue(MatchEnElbrProperty); }
            set { SetValue(MatchEnElbrProperty, value); }
        }
        public static readonly DependencyProperty MatchEnElbrProperty =
            DependencyProperty.Register(nameof(MatchEnElbr), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string SearchRangeText
        {
            get { return (string)GetValue(SearchRangeTextProperty); }
            set { SetValue(SearchRangeTextProperty, value); }
        }
        public static readonly DependencyProperty SearchRangeTextProperty =
            DependencyProperty.Register(nameof(SearchRangeText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string SecText
        {
            get { return (string)GetValue(SecTextProperty); }
            set { SetValue(SecTextProperty, value); }
        }
        public static readonly DependencyProperty SecTextProperty =
            DependencyProperty.Register(nameof(SecText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string SearchRangeElbr
        {
            get { return (string)GetValue(SearchRangeElbrProperty); }
            set { SetValue(SearchRangeElbrProperty, value); }
        }
        public static readonly DependencyProperty SearchRangeElbrProperty =
            DependencyProperty.Register(nameof(SearchRangeElbr), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string MaxLengthText
        {
            get { return (string)GetValue(MaxLengthTextProperty); }
            set { SetValue(MaxLengthTextProperty, value); }
        }
        public static readonly DependencyProperty MaxLengthTextProperty =
            DependencyProperty.Register(nameof(MaxLengthText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string MaxLengthElbr
        {
            get { return (string)GetValue(MaxLengthElbrProperty); }
            set { SetValue(MaxLengthElbrProperty, value); }
        }
        public static readonly DependencyProperty MaxLengthElbrProperty =
            DependencyProperty.Register(nameof(MaxLengthElbr), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string MinCnfrmNumText
        {
            get { return (string)GetValue(MinCnfrmNumTextProperty); }
            set { SetValue(MinCnfrmNumTextProperty, value); }
        }
        public static readonly DependencyProperty MinCnfrmNumTextProperty =
            DependencyProperty.Register(nameof(MinCnfrmNumText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string MinCnfrmNumElbr
        {
            get { return (string)GetValue(MinCnfrmNumElbrProperty); }
            set { SetValue(MinCnfrmNumElbrProperty, value); }
        }
        public static readonly DependencyProperty MinCnfrmNumElbrProperty =
            DependencyProperty.Register(nameof(MinCnfrmNumElbr), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string SubOffsetText
        {
            get { return (string)GetValue(SubOffsetTextProperty); }
            set { SetValue(SubOffsetTextProperty, value); }
        }
        public static readonly DependencyProperty SubOffsetTextProperty =
            DependencyProperty.Register(nameof(SubOffsetText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string CentiSecText
        {
            get { return (string)GetValue(CentiSecTextProperty); }
            set { SetValue(CentiSecTextProperty, value); }
        }
        public static readonly DependencyProperty CentiSecTextProperty =
            DependencyProperty.Register(nameof(CentiSecText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string SubOffsetElbr
        {
            get { return (string)GetValue(SubOffsetElbrProperty); }
            set { SetValue(SubOffsetElbrProperty, value); }
        }
        public static readonly DependencyProperty SubOffsetElbrProperty =
            DependencyProperty.Register(nameof(SubOffsetElbr), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string FastMatchText
        {
            get { return (string)GetValue(FastMatchTextProperty); }
            set { SetValue(FastMatchTextProperty, value); }
        }
        public static readonly DependencyProperty FastMatchTextProperty =
            DependencyProperty.Register(nameof(FastMatchText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string FastMatchElbr
        {
            get { return (string)GetValue(FastMatchElbrProperty); }
            set { SetValue(FastMatchElbrProperty, value); }
        }
        public static readonly DependencyProperty FastMatchElbrProperty =
            DependencyProperty.Register(nameof(FastMatchElbr), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        // OtherSettingPage

        public string OtherSettingsText
        {
            get { return (string)GetValue(OtherSettingsTextProperty); }
            set { SetValue(OtherSettingsTextProperty, value); }
        }
        public static readonly DependencyProperty OtherSettingsTextProperty =
            DependencyProperty.Register(nameof(OtherSettingsText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string LanguageText
        {
            get { return (string)GetValue(LanguageTextProperty); }
            set { SetValue(LanguageTextProperty, value); }
        }
        public static readonly DependencyProperty LanguageTextProperty =
            DependencyProperty.Register(nameof(LanguageText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string ThemeText
        {
            get { return (string)GetValue(ThemeTextProperty); }
            set { SetValue(ThemeTextProperty, value); }
        }
        public static readonly DependencyProperty ThemeTextProperty =
            DependencyProperty.Register(nameof(ThemeText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        // SpectrumPage

        public string ChannelText
        {
            get { return (string)GetValue(ChannelTextProperty); }
            set { SetValue(ChannelTextProperty, value); }
        }
        public static readonly DependencyProperty ChannelTextProperty =
            DependencyProperty.Register(nameof(ChannelText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public List<string> ChannelSel
        {
            get { return (List<string>)GetValue(ChannelSelProperty); }
            set { SetValue(ChannelSelProperty, value); }
        }
        public static readonly DependencyProperty ChannelSelProperty =
            DependencyProperty.Register(nameof(ChannelSel), typeof(List<string>), typeof(TextHelper), new PropertyMetadata(0));

        public string JumpSelText
        {
            get { return (string)GetValue(JumpSelTextProperty); }
            set { SetValue(JumpSelTextProperty, value); }
        }
        public static readonly DependencyProperty JumpSelTextProperty =
            DependencyProperty.Register(nameof(JumpSelText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public List<string> JumpSel
        {
            get { return (List<string>)GetValue(JumpSelProperty); }
            set { SetValue(JumpSelProperty, value); }
        }
        public static readonly DependencyProperty JumpSelProperty =
            DependencyProperty.Register(nameof(JumpSel), typeof(List<string>), typeof(TextHelper), new PropertyMetadata(0));

        public string ScaleSliderText
        {
            get { return (string)GetValue(ScaleSliderTextProperty); }
            set { SetValue(ScaleSliderTextProperty, value); }
        }
        public static readonly DependencyProperty ScaleSliderTextProperty =
            DependencyProperty.Register(nameof(ScaleSliderText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        // SpectrumSettingsPage

        public string SpectrumSettingsText
        {
            get { return (string)GetValue(SpectrumSettingsTextProperty); }
            set { SetValue(SpectrumSettingsTextProperty, value); }
        }
        public static readonly DependencyProperty SpectrumSettingsTextProperty =
            DependencyProperty.Register(nameof(SpectrumSettingsText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string DrawSpecText
        {
            get { return (string)GetValue(DrawSpecTextProperty); }
            set { SetValue(DrawSpecTextProperty, value); }
        }
        public static readonly DependencyProperty DrawSpecTextProperty =
            DependencyProperty.Register(nameof(DrawSpecText), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public string DrawSpecElbr
        {
            get { return (string)GetValue(DrawSpecElbrProperty); }
            set { SetValue(DrawSpecElbrProperty, value); }
        }
        public static readonly DependencyProperty DrawSpecElbrProperty =
            DependencyProperty.Register(nameof(DrawSpecElbr), typeof(string), typeof(TextHelper), new PropertyMetadata(0));

        public TextHelper()
        {
        }

        public void load_text_resouce()
        {
            // Generall
            OffText = AppResources.get_string("BDMatchUI/Common/Text_Off");
            OnText = AppResources.get_string("BDMatchUI/Common/Text_On");
            // AboutPage
            AboutHeader = AppResources.get_string("BDMatchUI/MainWindow/Navi/About/Content");
            var about_format = AppResources.get_string("BDMatchUI/AboutPage/Text_About");
            AboutText = string.Format(about_format, Constants.App_Version, Constants.FFmpeg_Version, Constants.FFTW_Version, Constants.CopyRight_Year);
            // DecodeSettingsPage
            DecodeSettingsText = AppResources.get_string("BDMatchUI/MainWindow/Navi/Decode_Settings/Content");
            FFTNumText = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/FFTNumText/Text");
            FFTNumElbr = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/FFTNumElbr/Text");
            MinFindDBText = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/MinFindDBText/Text");
            DecibelText = AppResources.get_string("BDMatchUI/Common/Text_Decibel");
            MinFindDBElbr = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/MinFindDBElbr/Text");
            OutputPCMText = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/OutputPCMText/Text");
            OutputPCMElbr = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/OutputPCMElbr/Text");
            ParallelDecodeText = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/ParallelDecodeText/Text");
            ParallelDecodeElbr = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/ParallelDecodeElbr/Text");
            VolMatchText = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/VolMatchText/Text");
            VolMatchElbr = AppResources.get_string("BDMatchUI/DeccodeSettingsPage/VolMatchElbr/Text");
            // HelpPage
            HelpText = AppResources.get_string("BDMatchUI/MainWindow/Navi/Help/Content");
            BatchProcessingHeader = AppResources.get_string("BDMatchUI/HelpPage/BatchProcessingHeader/Text");
            BatchProcessingText = AppResources.get_string("BDMatchUI/HelpPage/BatchProcessingText/Text");
            OneToMultiHeader = AppResources.get_string("BDMatchUI/HelpPage/OneToMultiHeader/Text");
            OneToMultiText = AppResources.get_string("BDMatchUI/HelpPage/OneToMultiText/Text");
            UsingRegexHeader = AppResources.get_string("BDMatchUI/HelpPage/UsingRegexHeader/Text");
            UsingRegexText = AppResources.get_string("BDMatchUI/HelpPage/UsingRegexText/Text");
            ExcludeSubExprHeader = AppResources.get_string("BDMatchUI/HelpPage/ExcludeSubExprHeader/Text");
            ExcludeSubExprText = AppResources.get_string("BDMatchUI/HelpPage/ExcludeSubExprText/Text");
            FastRegexHeader = AppResources.get_string("BDMatchUI/HelpPage/FastRegexHeader/Text");
            FastRegexText = AppResources.get_string("BDMatchUI/HelpPage/FastRegexText/Text");
            // LogsPage
            LogsText = AppResources.get_string("BDMatchUI/MainWindow/Navi/Logs/Content");
            // MainWindow
            Match_and_Result_Header = AppResources.get_string("BDMatchUI/MainWindow/Navi/Match_and_Result/Content");
            MatchPage_Item = AppResources.get_string("BDMatchUI/MainWindow/Navi/Match/Content");
            LogsPage_Item = AppResources.get_string("BDMatchUI/MainWindow/Navi/Logs/Content");
            SpectrumPage_Item = AppResources.get_string("BDMatchUI/MainWindow/Navi/Spectrum/Content");
            Settings_Header = AppResources.get_string("BDMatchUI/MainWindow/Navi/Settings/Content");
            MatchSettingsPage_Item = AppResources.get_string("BDMatchUI/MainWindow/Navi/Match_Settings/Content");
            DecodeSettingsPage_Item = AppResources.get_string("BDMatchUI/MainWindow/Navi/Decode_Settings/Content");
            SpectrumSettingsPage_Item = AppResources.get_string("BDMatchUI/MainWindow/Navi/Spectrum_Settings/Content");
            OtherSettingsPage_Item = AppResources.get_string("BDMatchUI/MainWindow/Navi/Other_Settings/Content");
            Others_Header = AppResources.get_string("BDMatchUI/MainWindow/Navi/Others/Content");
            HelpPage_Item = AppResources.get_string("BDMatchUI/MainWindow/Navi/Help/Content");
            AboutPage_Item = AppResources.get_string("BDMatchUI/MainWindow/Navi/About/Content");
            // MatchPage
            SubPathPlaceholderText = AppResources.get_string("BDMatchUI/MatchPage/SubPath/PlaceholderText");
            TVPathPlaceholderText = AppResources.get_string("BDMatchUI/MatchPage/TVPath/PlaceholderText");
            BDPathPlaceholderText = AppResources.get_string("BDMatchUI/MatchPage/BDPath/PlaceholderText");
            SubText = AppResources.get_string("BDMatchUI/Common/Text_Sub");
            TVText = AppResources.get_string("BDMatchUI/Common/Text_TV");
            BDText = AppResources.get_string("BDMatchUI/Common/Text_BD");
            MatchButtonContent = AppResources.get_string("BDMatchUI/Common/Text_Match");
            MatchProgressText = AppResources.get_string("BDMatchUI/Common/Text_Ready");
            LogButtonContent = AppResources.get_string("BDMatchUI/MatchPage/LogButton/Content");
            // MatchSettingsPage
            MatchSettingsText = AppResources.get_string("BDMatchUI/MainWindow/Navi/Match_Settings/Content");
            MatchEnText = AppResources.get_string("BDMatchUI/MatchSettingsPage/MatchEnText/Text");
            MatchEnElbr = AppResources.get_string("BDMatchUI/MatchSettingsPage/MatchEnElbr/Text");
            SearchRangeText = AppResources.get_string("BDMatchUI/MatchSettingsPage/SearchRangeText/Text");
            SecText = AppResources.get_string("BDMatchUI/Common/Text_Sec");
            SearchRangeElbr = AppResources.get_string("BDMatchUI/MatchSettingsPage/SearchRangeElbr/Text");
            MaxLengthText = AppResources.get_string("BDMatchUI/MatchSettingsPage/MaxLengthText/Text");
            MaxLengthElbr = AppResources.get_string("BDMatchUI/MatchSettingsPage/MaxLengthElbr/Text");
            MinCnfrmNumText = AppResources.get_string("BDMatchUI/MatchSettingsPage/MinCnfrmNumText/Text");
            MinCnfrmNumElbr = AppResources.get_string("BDMatchUI/MatchSettingsPage/MinCnfrmNumElbr/Text");
            SubOffsetText = AppResources.get_string("BDMatchUI/Common/Text_SubTimeDelay");
            CentiSecText = AppResources.get_string("BDMatchUI/Common/Text_CentiSec");
            SubOffsetElbr = AppResources.get_string("BDMatchUI/MatchSettingsPage/SubOffsetElbr/Text");
            FastMatchText = AppResources.get_string("BDMatchUI/MatchSettingsPage/FastMatchText/Text");
            FastMatchElbr = AppResources.get_string("BDMatchUI/MatchSettingsPage/FastMatchElbr/Text");
            // OtherSettingPage
            OtherSettingsText = AppResources.get_string("BDMatchUI/MainWindow/Navi/Other_Settings/Content");
            LanguageText = AppResources.get_string("BDMatchUI/OtherSettingsPage/LanguageText/Text");
            ThemeText = AppResources.get_string("BDMatchUI/OtherSettingsPage/ThemeText/Text");
            // SpectrumPage
            ChannelText = AppResources.get_string("BDMatchUI/SpectrumPage/ChannelText/Text");
            ChannelSel = new List<string> {
                AppResources.get_string("BDMatchUI/SpectrumPage/Channel_Left"),
                AppResources.get_string("BDMatchUI/SpectrumPage/Channel_Right"),
                "2", "3", "4", "5", "6", "7" };
            JumpSelText = AppResources.get_string("BDMatchUI/SpectrumPage/JumpSelText/Text");
            JumpSel = new List<string> {
                AppResources.get_string("BDMatchUI/SpectrumPage/JumpSel_Time"),
                AppResources.get_string("BDMatchUI/SpectrumPage/JumpSel_Timeline"),
            };
            ScaleSliderText = AppResources.get_string("BDMatchUI/SpectrumPage/ScaleSliderText/Text");
            // SpectrumSettingsPage
            SpectrumSettingsText = AppResources.get_string("BDMatchUI/MainWindow/Navi/Spectrum_Settings/Content");
            DrawSpecText = AppResources.get_string("BDMatchUI/SpectrumSettingsPage/DrawSpecText/Text");
            DrawSpecElbr = AppResources.get_string("BDMatchUI/SpectrumSettingsPage/DrawSpecElbr/Text");
        }

    }
}

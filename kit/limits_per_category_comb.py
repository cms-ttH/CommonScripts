import numpy as np
from ROOT import TH2F, TCanvas, gStyle, TLatex, TAxis, TLine, TGraphErrors, TGraphAsymmErrors, TLegend, kGreen, kYellow, gPad, gROOT
gROOT.SetBatch(True)

fontsize = 0.04

def limits_dl():
    cat_names = [
        '3 jets, 3 b-tags',
        '#geq 4 jets, 3 b-tags (low BDT)',
        '#geq 4 jets, 3 b-tags (high BDT)',
        '#geq 4 jets, #geq 4 b-tags (low BDT)',
        '#geq 4 jets, #geq 4 b-tags (high BDT)',
        'DL combined']
    obs          = np.array( [22.1566105651, 12.6068934797, 5.24734395819, 10.6120004825, 9.63585946011, 3.1800042632  ] )
    upper2sig    = np.array( [31.1722412109, 12.6767692566, 9.82194709778, 13.9833049774, 7.94944763184, 3.67880916595 ] )
    upper1sig    = np.array( [12.9079284668, 5.43609046936, 4.2118768692,  5.64859676361, 3.19450187683, 1.53992033005 ] )
    expect       = np.array( [25.90625,      11.65625,      9.03125,       10.34375,      5.765625,      3.359375      ] )
    lower1sig    = np.array( [8.17812347412, 3.49255275726, 2.72793769836, 3.43452739716, 1.9144115448,  1.0274810791  ] )
    lower2sig    = np.array( [13.0037231445, 5.62322998047, 4.39215087891, 5.39410400391, 3.00668334961, 1.63375854492 ] )
    nchannels=len(cat_names)

    xmin = 0.7
    xmax = 180

    c,h = draw_canvas_histo( nchannels, xmin, xmax, "95% CL limit on #mu = #sigma/#sigma_{SM} at m_{H} = 125 GeV", cat_names, "DL" )
    c.SetLogx()
    
    go,ge1,ge2,l = draw_limits_per_category( nchannels, xmin, xmax, obs, expect, upper1sig, lower1sig, upper2sig, lower2sig )

    #draw_disclaimer()

    c.RedrawAxis()    
    c.Modified()
    c.Update()
    c.SaveAs( "HIG-16-038_limits_DL.pdf" )



def limits_sl():
    cat_names = ['4 jets, 4 b-tags (low BDT)',
                 '4 jets, 4 b-tags (high BDT)',
                 '5 jets, #geq 4 b-tags (low BDT)',
                 '5 jets, #geq 4 b-tags (high BDT)',
                 '#geq 6 jets, 3 b-tags (low BDT)',
                 '#geq 6 jets, 3 b-tags (high BDT)',
                 '#geq 6 jets, #geq 4 b-tags (low BDT)',
                 '#geq 6 jets, #geq 4 b-tags (high BDT)',
                 'LJ combined']
    
    obs          = np.array([ 46.8887337787, 12.8040440625, 19.9903200582, 6.0313550296,  12.0941251601, 5.80214226433, 9.61154177058, 6.12780192127, 1.83107995924  ] )
    upper2sig    = np.array([ 60.3337860107, 15.7014541626, 19.4860420227, 7.17677307129, 18.2545623779, 8.03229999542, 10.778711319,  5.06503868103, 2.29233074188  ] )
    upper1sig    = np.array([ 25.9850158691, 6.59627723694, 8.28972434998, 2.99532413483, 7.99176597595, 3.47670173645, 4.53673934937, 2.08750295639, 0.970508098602 ] )
    expect       = np.array([ 53.0,          13.90625,      17.1875,       6.109375,      18.0625,       7.71875,       9.40625,       4.328125,      2.1171875      ] )
    lower1sig    = np.array([ 17.0315551758, 4.13298034668, 5.2991027832,  1.84536933899, 5.17089271545, 2.30846405029, 2.86402893066, 1.32377767563, 0.629234552383 ] )
    lower2sig    = np.array([ 26.9140625,    6.65435791016, 8.42590332031, 2.97116088867, 8.43151855469, 3.69354248047, 4.61126708984, 2.10488891602, 1.0131072998   ] )
    nchannels=len(cat_names)

    xmin = 0.7
    xmax = 180

    c,h = draw_canvas_histo( nchannels, xmin, xmax, "95% CL limit on #mu = #sigma/#sigma_{SM} at m_{H} = 125 GeV", cat_names, "SL" )
    c.SetLogx()
    
    go,ge1,ge2,l = draw_limits_per_category( nchannels, xmin, xmax, obs, expect, upper1sig, lower1sig, upper2sig, lower2sig )

    #draw_disclaimer()

    c.RedrawAxis()    
    c.Modified()
    c.Update()
    c.SaveAs( "HIG-16-038_limits_SL.pdf" )


def draw_limits_per_category( nchannels, xmin, xmax, obs, expect, upper1sig, lower1sig, upper2sig, lower2sig ):

    channel = np.array( [ 3.*nchannels - 1.5 - 3.*i for i in range( 0, nchannels ) ] )
    ey      = np.array( [ 0.8 for i in range( 0, nchannels ) ] )
    zero    = np.zeros( nchannels )

    gexpect1sig = TGraphAsymmErrors( nchannels, expect, channel, lower1sig, upper1sig, ey, ey )
    gexpect1sig.SetFillColor( kGreen )
    gexpect1sig.SetLineWidth( 2 )
    gexpect1sig.SetLineStyle( 2 )
    
    gexpect2sig = TGraphAsymmErrors( nchannels, expect, channel, lower2sig, upper2sig, ey, ey )
    gexpect2sig.SetFillColor( kYellow )
    gexpect2sig.SetLineWidth( 2 )
    gexpect2sig.SetLineStyle( 2 )

    gexpect2sig.Draw("2")
    gexpect1sig.Draw("2")

    gobs = TGraphErrors( nchannels, obs, channel, zero, ey )
    gobs.SetMarkerStyle( 21 )
    gobs.SetMarkerSize( 1.5 )
    gobs.SetLineWidth( 2 )
    gobs.Draw("pz")

    # dashed line at median expected limits
    l = TLine()
    l.SetLineStyle( 2 )
    l.SetLineWidth( 2 )
    for bin in range( nchannels ):
        l.DrawLine( expect[bin], channel[bin]-ey[bin], expect[bin], channel[bin]+ey[bin] )

    # line to separate individual and combined limits
    l.SetLineStyle( 1 )
    l.SetLineWidth( 1 )
    l.DrawLine( xmin, 0, xmax, 0 )

    # legend
    x1 = gStyle.GetPadLeftMargin()+0.01
    y2 = 1-gStyle.GetPadTopMargin()-0.01
    leg = TLegend( x1, y2-0.17, x1+0.25, y2 )
    leg.SetFillColor( 4000 )
    leg.AddEntry( gexpect1sig, "Expected #pm1#sigma", "FL" )
    leg.AddEntry( gexpect2sig, "Expected #pm2#sigma", "FL" )
    leg.AddEntry( gobs,        "Observed", "pl" )
    leg.Draw()

    return gobs, gexpect1sig, gexpect2sig, leg


def draw_canvas_histo( nchannels, xmin, xmax, title, cat_names, channelID ):
    c = TCanvas( "c", "Canvas", 800, 600 )
    c.Draw()
    
    h = TH2F( "h", "", 10, xmin, xmax, 3*nchannels, 0, 3*nchannels )
    h.Draw()
    h.SetStats( 0 )
    h.SetXTitle( title )

    yaxis = h.GetYaxis()
    yaxis.SetLabelSize( 0.05 )

    for ibin in range( 0, nchannels ):
        yaxis.SetBinLabel( 3*nchannels-1-3*ibin, cat_names[ibin] )

    pub = TLatex();
    pub.SetNDC()
    pub.SetTextFont( 42 )
    pub.SetTextSize( 0.045 )
    pub.SetTextAlign( 11 )
    pub.DrawLatex( gStyle.GetPadLeftMargin(), 0.92, "#scale[1.2]{#bf{CMS}} #it{Preliminary}" )

    lumi = TLatex();
    lumi.SetNDC()
    lumi.SetTextFont( 42 )
    lumi.SetTextSize( 0.035 )
    lumi.SetTextAlign( 31 )
    if channelID is "DL":
        lumi.DrawLatex( 1-gStyle.GetPadRightMargin(), 0.92, "11.4 - 12.9 fb^{-1} (13 TeV)" )
    elif channelID is "SL":
        lumi.DrawLatex( 1-gStyle.GetPadRightMargin(), 0.92, "12.9 fb^{-1} (13 TeV)" )

    return c,h

def draw_disclaimer():
    # disclaimer
    t = TLatex();
    t.SetNDC()
    t.SetTextSize( 0.1 )
    t.SetTextAlign( 22 )
    t.SetTextAngle( 45 )
    t.DrawText( 0.5, 0.5, "FAKE VALUES" )
    
def my_style():
    
    gStyle.SetLabelSize( fontsize, "x" );
    gStyle.SetLabelSize( fontsize, "y" );
    gStyle.SetLabelSize( fontsize, "z" );

    gStyle.SetTitleFontSize( 1.5*fontsize );
    gStyle.SetTitleSize( fontsize, "x" );
    gStyle.SetTitleSize( fontsize, "y" );
    gStyle.SetTitleSize( fontsize, "z" );
    gStyle.SetTickLength(0,"Y")

    gStyle.SetTitleOffset( 1.5, "xy" );
    gStyle.SetTitleFont( 62, "bla" );

    gStyle.SetPadBottomMargin(0.15);
    gStyle.SetPadTopMargin(0.10);
    gStyle.SetPadLeftMargin(0.32);
    gStyle.SetPadRightMargin(0.05);

    gStyle.SetStatX( 0.88 );
    gStyle.SetStatY( 0.87 );
    gStyle.SetNdivisions( 505 );

    gStyle.SetCanvasColor(-1); 
    gStyle.SetPadColor(-1); 
    gStyle.SetFrameFillColor(-1); 
    gStyle.SetTitleFillColor(-1); 
    gStyle.SetFillColor(-1); 
    gStyle.SetFillStyle(4000); 
    gStyle.SetStatStyle(0); 
    gStyle.SetTitleStyle(0); 
    gStyle.SetCanvasBorderSize(0); 
    gStyle.SetFrameBorderSize(0); 
    gStyle.SetLegendBorderSize(0); 
    gStyle.SetStatBorderSize(0); 
    gStyle.SetTitleBorderSize(0); 
    
if __name__ == '__main__':
    my_style()
    limits_dl()
    limits_sl()


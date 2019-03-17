//
//  ViewController.swift
//  NC-2Ch
//
//  Created by Abhishek Sehgal on 5/12/17.
//  Copyright © 2017 SIPLab. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    
    @IBOutlet weak var storeSwitch: UISwitch!
    @IBOutlet weak var buttonStart: UIButton!
    @IBOutlet weak var buttonStop:  UIButton!
    @IBOutlet weak var labelTime:   UILabel!
    @IBOutlet weak var storeLabel: UILabel!
    @IBOutlet weak var infoView: UITextView!
    @IBOutlet weak var noiseLabel: UILabel!
    
    
    var timer: Timer! = Timer()
    
    func buttonEnable(set: Bool) {
        buttonStart.isEnabled   = set
        storeSwitch.isEnabled   = set
        buttonStop.isEnabled    = !set
    }
    
    @IBAction func buttonStartPress(_ sender: Any) {
        timer = Timer.scheduledTimer(withTimeInterval: 0.5,
                                     repeats: true,
                                     block: {
                                        _ in self.labelTime.text = String(format: "Frame Processing Time: %.2f ms", 1000*audioController.timeBuffer.movingAverage, audioController.classLabel)
                                        switch audioController.classLabel{
                                        case 1:
                                            self.noiseLabel.text = String(format: "Detected Noise Class: Babble")
                                        case 2:
                                            self.noiseLabel.text = String(format: "Detected Noise Class: Driving Car")
                                        case 3:
                                            self.noiseLabel.text = String(format: "Detected Noise Class: Machinery")
                                        default:
                                            self.noiseLabel.text = String(format: "Detected Noise Class: ")
                                        }
                                        
        });

        buttonEnable(set: false);
        audioController.start(storeSwitch.isOn);
        
    }
    
    @IBAction func buttonStopPress(_ sender: Any) {
        timer.invalidate()
        buttonEnable(set: true);
        audioController.stop();
        
    }
   
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        storeSwitch.setOn(false, animated: false)
        storeLabel.text = "Record Audio"
        buttonEnable(set: true);
        audioController = IOSAudioController();
        labelTime.text  = "Frame Processing Time:"
        infoView.text   = audioController.info
        noiseLabel.text = "Detected Noise Class:"
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}


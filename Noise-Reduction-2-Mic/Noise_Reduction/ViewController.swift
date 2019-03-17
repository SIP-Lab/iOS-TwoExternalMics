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
    @IBOutlet weak var noiseReductionSelect: UISegmentedControl!
    
    
    var timer: Timer! = Timer()
    
    func buttonEnable(set: Bool) {
        buttonStart.isEnabled   = set
        storeSwitch.isEnabled   = set
        buttonStop.isEnabled    = !set
        noiseReductionSelect.isEnabled = set
    }
    
    @IBAction func buttonStartPress(_ sender: Any) {
        timer = Timer.scheduledTimer(withTimeInterval: 0.5,
                                     repeats: true,
                                     block: {
                                        _ in self.labelTime.text = String(format: "Frame Processing Time: %.2f ms", 1000*audioController.timeBuffer.movingAverage)
                                        
                                        
        });

        audioController.noiseReductionSelect = Int32(self.noiseReductionSelect.selectedSegmentIndex)
        buttonEnable(set: false);
        audioController.start(storeSwitch.isOn);
        
    }
    
    @IBAction func buttonStopPress(_ sender: Any) {
        timer.invalidate()
        buttonEnable(set: true);
        audioController.stop();
        labelTime.text = "Frame Processing Time:"
        
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
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}


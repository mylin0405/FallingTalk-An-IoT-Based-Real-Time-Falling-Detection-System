隊名：大貢丸
專題題目：FallingTalk: An IoT-Based Real-Time Falling Detection System
組長：
0617052 林明佑
組員：
0616058 黃胤錚

專題摘要：

近年來，隨著高齡化的現象，造成老年人獨居的問題越發嚴重，為了減少照護者負擔以及避免意外的情況的發生，我們會需要一個自動化的方法來幫助老年人減少跌倒所造成的致命傷害。在此Project，我們結合了IoT solution: IoTtalk和Image-Based solution提出了「FallingTalk」這個系統，FallingTalk能夠以single-camera的方式進行real-time的跌倒偵測，並即時的以文字的方式用Line來提醒看護者。在FallingTalk中，Image-Based部分首先會使用Motion History Image(MHI)係數檢測人體活動劇烈程度，接下來會透過Background Subtraction取出human silhouettes，並以此輪廓檢測出人體姿勢特徵，結合MHI係數以及人體姿勢特徵，最後會將人類活動情形區分成三種情況，(1)正常情況，如走路中、跑步中、正常坐下等等，並以綠框在畫面上輔助顯示(2)低危險情況，如從椅子上爬起、從躺下狀態爬起等等，並以橘框在畫面上輔助顯示(3)高危險情況，如跌倒或滑倒，並以紅框在畫面上輔助顯示；IoT-Based部分，會將Image-Based所檢測出的結果透過IoTtalk傳遞至使用者的Line，實驗結果FallingTalk具有92.1%的準確率，並且傳送至用戶端Line延遲在2秒以內。
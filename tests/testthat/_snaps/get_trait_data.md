# Get data from simulation output

    Code
      colnames(trait_data)
    Output
       [1] "intake"     "energy"     "sF"         "sH"         "sN"        
       [6] "x"          "y"          "xn"         "yn"         "assoc"     
      [11] "t_infec"    "src_infect" "moved"      "gen"       

---

    Code
      colnames(as.data.frame(networks[[1]]))
    Output
       [1] "intake"             "energy"             "sF"                
       [4] "sH"                 "sN"                 "x"                 
       [7] "y"                  "xn"                 "yn"                
      [10] "assoc"              "t_infec"            "src_infect"        
      [13] "moved"              "gen"                "id"                
      [16] "popsize"            "range_food"         "range_agents"      
      [19] "range_move"         "handling_time"      "pTransmit"         
      [22] "p_v_transmit"       "initialInfections"  "costInfect"        
      [25] "infect_percent"     "vertical_infection" "reprod_threshold"  
      [28] "dispersal"          "mProb"              "mSize"             
      [31] "scenario"           "genmax"             "g_patho_init"      
      [34] "spillover_rate"     "nItems"             "landsize"          
      [37] "nClusters"          "clusterSpread"      "tmax"              
      [40] "regen_time"         "social_strat"      

---

    Code
      colnames(movement)
    Output
      [1] "time" "x"    "y"    "id"   "type"


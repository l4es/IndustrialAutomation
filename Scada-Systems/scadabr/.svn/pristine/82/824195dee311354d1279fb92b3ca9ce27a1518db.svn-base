/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package br.org.scadabr.web.mvc.controller.dataPoints;

import br.org.scadabr.DataType;
import com.serotonin.mango.vo.DataPointVO;
import java.util.Arrays;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

/**
 *
 * @author aploese
 */
@Controller
@Scope("request")
@RequestMapping(value = "/dataPoints")
public class ControllerDp {

    @ModelAttribute
    protected void getModel(Model model) {
        model.addAttribute("dataTypes", DataType.values());
    }

    @RequestMapping(method = RequestMethod.GET)
    public String showForm() {
        return "dataPoints";
    }

}
 

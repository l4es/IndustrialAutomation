package br.org.scadabr.web.mvc.controller;


import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;

@Deprecated //TODO <mvc:view-controller path="/" view-name="index"/> springDispatcher-servlet.xml breaks JSONRPC4J so this is the workaround ... 
@Controller
@RequestMapping("/")
@Scope("request")
class WelcomeController {
    
    @RequestMapping(method = RequestMethod.GET)
    protected String showForm() throws Exception {
        return "index";
    }
    
}
